import com.secomba.base4k.*;

import javax.swing.JOptionPane;

public class Example {
	public static void main(String[] args) {
		String unicodeConsoleReplacement = "";
		
		String plain = JOptionPane.showInputDialog("Please enter a plaintext");
		
		String encoded = Base4K.encode(plain.getBytes());
		unicodeConsoleReplacement += "## encoded ##\n";
		unicodeConsoleReplacement += encoded+"\n";
		unicodeConsoleReplacement += "## decoded ##\n";
		
		try {
			String decoded = new String(Base4K.decode(encoded));
			unicodeConsoleReplacement += decoded+"\n";
		} catch(DecodingFailedException e) {
			unicodeConsoleReplacement += "Provided string is not a valid base4k-encoding!\n";
		}
		
		JOptionPane.showMessageDialog(null,unicodeConsoleReplacement);
	}
}
