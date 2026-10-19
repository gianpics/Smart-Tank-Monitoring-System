package esiot.backend;

import io.vertx.core.AbstractVerticle;
import io.vertx.mqtt.MqttClient;

public class MqttService extends AbstractVerticle {

    private static final String BROKER = "broker.mqtt-dashboard.com";
    private static final int PORT = 1883;
    private static final String TOPIC = "TMS-esiot-2025-Water-Level";

    private final SystemState state;

    public MqttService(SystemState state) {
        this.state = state;
    }

    @Override
    public void start() {
        MqttClient client = MqttClient.create(vertx);

        client.publishHandler(msg -> {
            String payload = msg.payload().toString();
            try {
                float value = Float.parseFloat(payload);
                state.addMeasurement(value);
                log("Received: " + value);
            } catch (NumberFormatException e) {
                log("Invalid payload: " + payload);
            }
        });

        client.connect(PORT, BROKER, ar -> {
            if (ar.succeeded()) {
                log("Connected to broker");
                client.subscribe(TOPIC, 0, ar2 -> {
                    if (ar2.succeeded()) {
                        log("Subscribed to " + TOPIC);
                    } else {
                        log("Subscribe failed: " + ar2.cause().getMessage());
                    }
                });
            } else {
                log("Connection failed: " + ar.cause().getMessage());
            }
        });
    }

    private void log(String msg) {
        System.out.println("[MQTT SERVICE] " + msg);
    }
}