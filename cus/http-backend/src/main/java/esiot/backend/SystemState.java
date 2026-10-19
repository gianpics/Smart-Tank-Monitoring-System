package esiot.backend;

import java.util.LinkedList;

public class SystemState {

    public enum SystemMode { AUTOMATIC, MANUAL, UNCONNECTED }

    private static final int MAX_SIZE = 10;

    private SystemMode mode;
    private int valveOpeningPercentage;
    private LinkedList<DataPoint> measurements;
    private long lastTmsMessageTime;

    public SystemState() {
        this.mode = SystemMode.AUTOMATIC;
        this.valveOpeningPercentage = 0;
        this.measurements = new LinkedList<>();
        this.lastTmsMessageTime = System.currentTimeMillis();
    }

    public void addMeasurement(float value) {
        long time = System.currentTimeMillis();
        measurements.addFirst(new DataPoint(value, time));
        if (measurements.size() > MAX_SIZE) {
            measurements.removeLast();
        }
        this.lastTmsMessageTime = time;
    }

    public float getLatestValue() {
        return measurements.isEmpty() ? 0 : measurements.getFirst().getValue();
    }

    public LinkedList<DataPoint> getMeasurements() {
        return measurements;
    }

    public SystemMode getMode() {
        return mode;
    }

    public void setMode(SystemMode mode) {
        this.mode = mode;
    }

    public int getValveOpeningPercentage() {
        return valveOpeningPercentage;
    }

    public void setValveOpeningPercentage(int percentage) {
        this.valveOpeningPercentage = percentage;
    }

    public long getLastTmsMessageTime() {
        return lastTmsMessageTime;
    }
}
