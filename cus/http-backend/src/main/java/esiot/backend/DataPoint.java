package esiot.backend;

class DataPoint {
	private float value;
	private long time;
	
	public DataPoint(float value, long time) {
		this.value = value;
		this.time = time;
	}
	
	public float getValue() {
		return value;
	}
	
	public long getTime() {
		return time;
	}

}
