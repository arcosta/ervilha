public class Hello {
	public static void main(String args[]){
		System.out.println("Opaaaaa");
		while(true){
			System.out.println("Opaaaaa2");
			try{
				Thread.sleep(2000);
			}catch(InterruptedException e){
			}
		}
	}
}
