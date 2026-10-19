package esiot.backend;

import io.vertx.core.Vertx;

public class RunBackend {

    static final String SERIAL_PORT = "COM14";
    static final int SERIAL_RATE = 115200;

    public static void main(String[] args) {
        Vertx vertx = Vertx.vertx();
        SystemState state = new SystemState();

        SerialCommChannel serial = null;
        try {
            serial = new SerialCommChannel(SERIAL_PORT, SERIAL_RATE);
            WcsAgent wcsAgent = new WcsAgent(serial, state);
            wcsAgent.start();
            System.out.println("[BACKEND] Serial connected on " + SERIAL_PORT);
        } catch (Exception e) {
            System.out.println("[BACKEND] Serial not available: " + e.getMessage());
            System.out.println("[BACKEND] Running without WCS serial connection.");
        }

        vertx.deployVerticle(new MqttService(state));
        vertx.deployVerticle(new DataService(state, serial));
    }
}
