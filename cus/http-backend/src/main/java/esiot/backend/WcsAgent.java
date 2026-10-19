package esiot.backend;

import esiot.backend.SystemState.SystemMode;

public class WcsAgent extends Thread {

    private final SerialCommChannel channel;
    private final SystemState state;

    public WcsAgent(SerialCommChannel channel, SystemState state) {
        this.channel = channel;
        this.state = state;
    }

    @Override
    public void run() {
        while (true) {
            try {
                String msg = channel.receiveMsg();
                // log("RAW: " + msg);
                parseWcsMsg(msg);
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
                break;
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    private void parseWcsMsg(String msg) {
        msg = msg.trim();
        if (!msg.startsWith("<") || !msg.endsWith(">")) return;

        String inner = msg.substring(1, msg.length() - 1);
        int sep = inner.indexOf(';');
        if (sep < 0) return;

        String stateStr = inner.substring(0, sep);
        String openingStr = inner.substring(sep + 1);

        SystemMode newMode = parseMode(stateStr);
        if (newMode == null) return;

        int opening;
        try {
            opening = Integer.parseInt(openingStr);
        } catch (NumberFormatException e) {
            return;
        }
        if (opening < 0 || opening > 100)
            return;

        SystemMode currentMode = state.getMode();

        // mode changed by WCS
        if (newMode != currentMode) {
            state.setMode(newMode);
            log("Mode changed to " + newMode + " by WCS");
        }

        if (newMode == SystemMode.MANUAL && opening != state.getValveOpeningPercentage()) {
            state.setValveOpeningPercentage(opening);
            log("Valve opening updated to " + opening + "% by WCS");
        }
    }

    private SystemMode parseMode(String s) {
        return switch (s) {
            case "AUTO"        -> SystemMode.AUTOMATIC;
            case "MANUAL"      -> SystemMode.MANUAL;
            case "UNCONNECTED" -> SystemMode.UNCONNECTED;
            default            -> null;
        };
    }

    private void log(String msg) {
        System.out.println("[WCS AGENT] " + msg);
    }
}
