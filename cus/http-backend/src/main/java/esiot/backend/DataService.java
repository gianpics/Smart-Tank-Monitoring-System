package esiot.backend;

import io.vertx.core.AbstractVerticle;
import io.vertx.core.http.HttpServerResponse;
import io.vertx.core.json.JsonArray;
import io.vertx.core.json.JsonObject;
import io.vertx.ext.web.Router;
import io.vertx.ext.web.RoutingContext;
import io.vertx.ext.web.handler.BodyHandler;
import io.vertx.ext.web.handler.CorsHandler;
import esiot.backend.SystemState.SystemMode;
import java.util.Set;

public class DataService extends AbstractVerticle {

    private static final int PORT = 8080;

    // FSM thresholds
    private static final float L1 = 0.30f;  //m
    private static final float L2 = 0.40f;  // m
    private static final long T1 = 5000;    // ms
    private static final long T2 = 5000;    // ms

    private final SystemState state;
    private final SerialCommChannel serial;

    // FSM timers
    private long aboveL1Since = -1;
    private SystemMode modeBeforeUnconnected = SystemMode.AUTOMATIC;

    public DataService(SystemState state, SerialCommChannel serial) {
        this.state = state;
        this.serial = serial;
    }

    @Override
    public void start() {
        Router router = Router.router(vertx);
        router.route().handler(CorsHandler.create()
                .allowedMethods(Set.of(io.vertx.core.http.HttpMethod.GET, io.vertx.core.http.HttpMethod.POST, io.vertx.core.http.HttpMethod.OPTIONS))
                .allowedHeader("Content-Type"));
        router.route().handler(BodyHandler.create());

        router.get("/api/state").handler(this::handleGetState);
        router.post("/api/mode").handler(this::handleSetMode);
        router.post("/api/valve").handler(this::handleSetValve);

        vertx.createHttpServer()
            .requestHandler(router)
            .listen(PORT);

        // Define periodic task after a 3 sec wait to avoid undesired
        // behaviours during WCS board init (DTR toggle)
        vertx.setTimer(3000, id -> {
            // FSM tick - runs every 500ms
            vertx.setPeriodic(500, tid -> fsmTick());

            // T2 watchdog - check TMS connectivity
            vertx.setPeriodic(1000, tid -> checkConnectivity());
        });

        log("Service ready on port: " + PORT);
    }

    // --- FSM ---

    private void fsmTick() {
        if (state.getMode() != SystemMode.AUTOMATIC) return;

        // larger value = larger valve opening
        float level = state.getLatestValue();
        long now = System.currentTimeMillis();

        if (level >= L2) {
            aboveL1Since = -1;
            setValveAndNotify(100);
        } else if (level >= L1) {
            if (aboveL1Since < 0) {
                aboveL1Since = now;
            } else if (now - aboveL1Since >= T1) {
                setValveAndNotify(50);
            }
        } else {
            aboveL1Since = -1;
            setValveAndNotify(0);
        }
    }

    private void checkConnectivity() {
        long elapsed = System.currentTimeMillis() - state.getLastTmsMessageTime();
        SystemMode currentMode = state.getMode();

        if (elapsed > T2 && currentMode != SystemMode.UNCONNECTED) {
            modeBeforeUnconnected = currentMode;
            state.setMode(SystemMode.UNCONNECTED);
            sendToWcs(SystemMode.UNCONNECTED, state.getValveOpeningPercentage());
            log("TMS unreachable - entering UNCONNECTED");
        } else if (elapsed <= T2 && currentMode == SystemMode.UNCONNECTED) {
            state.setMode(modeBeforeUnconnected);
            sendToWcs(modeBeforeUnconnected, state.getValveOpeningPercentage());
            log("TMS back online - returning to " + modeBeforeUnconnected);
        }
    }

    private void setValveAndNotify(int percentage) {
        if (percentage == state.getValveOpeningPercentage()) 
            return;
        state.setValveOpeningPercentage(percentage);
        sendToWcs(SystemMode.AUTOMATIC, percentage);
    }

    private void sendToWcs(SystemMode mode, int percentage) {
        if (serial == null) 
            return;
        String modeStr = switch (mode) {
            case AUTOMATIC   -> "AUTO";
            case MANUAL      -> "MANUAL";
            case UNCONNECTED -> "UNCONNECTED";
        };
        serial.sendMsg("<" + modeStr + ";" + percentage + ">");
    }

    // --- HTTP handlers ---

    private void handleGetState(RoutingContext ctx) {
        JsonArray measurements = new JsonArray();
        for (DataPoint p : state.getMeasurements()) {
            measurements.add(new JsonObject()
                    .put("time", p.getTime())
                    .put("value", p.getValue()));
        }

        JsonObject response = new JsonObject()
                .put("mode", state.getMode().toString())
                .put("valveOpening", state.getValveOpeningPercentage())
                .put("measurements", measurements);

        ctx.response()
                .putHeader("content-type", "application/json")
                .end(response.encodePrettily());
    }

    private void handleSetMode(RoutingContext ctx) {
        JsonObject body = ctx.getBodyAsJson();
        if (body == null) { 
            sendError(400, ctx.response()); 
            return; 
        }

        String modeStr = body.getString("mode");
        if (modeStr == null) { 
            sendError(400, ctx.response()); 
            return; 
        }

        SystemMode newMode = switch (modeStr.toUpperCase()) {
            case "AUTOMATIC", "AUTO" -> SystemMode.AUTOMATIC;
            case "MANUAL"            -> SystemMode.MANUAL;
            default                  -> null;
        };

        if (newMode == null) { 
            sendError(400, ctx.response()); 
            return; 
        }

        state.setMode(newMode);
        sendToWcs(newMode, state.getValveOpeningPercentage());

        log("Mode set to " + newMode + " by DBS");
        ctx.response().setStatusCode(200).end();
    }

    private void handleSetValve(RoutingContext ctx) {
        if (state.getMode() != SystemMode.MANUAL) {
            sendError(403, ctx.response());
            return;
        }

        JsonObject body = ctx.getBodyAsJson();
        if (body == null) { 
            sendError(400, ctx.response()); 
            return; 
        }

        Integer opening = body.getInteger("opening");
        if (opening == null || opening < 0 || opening > 100) {
            sendError(400, ctx.response());
            return;
        }

        if (opening != state.getValveOpeningPercentage()) {
            state.setValveOpeningPercentage(opening);
            sendToWcs(SystemMode.MANUAL, opening);
            log("Valve set to " + opening + "% by DBS");
        }

        ctx.response().setStatusCode(200).end();
    }

    private void sendError(int statusCode, HttpServerResponse response) {
        response.setStatusCode(statusCode).end();
    }

    private void log(String msg) {
        System.out.println("[DATA SERVICE] " + msg);
    }
}
