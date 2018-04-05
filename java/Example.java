import com.secomba.base4k.*;

import javax.swing.JOptionPane;

public class Example {
	public static void main(String[] args) {
		String unicodeConsoleReplacement = "";
		
		Base4K base4k = new Base4K(Base4KVersion.V2);

		String plain = JOptionPane.showInputDialog("Please enter a plaintext");
		
		String encoded = base4k.encode(plain.getBytes());
		unicodeConsoleReplacement += "## encoded ##\n";
		unicodeConsoleReplacement += encoded+"\n";
		unicodeConsoleReplacement += "## decoded ##\n";
		
		try {
			String decoded = new String(base4k.decode(encoded));
			unicodeConsoleReplacement += decoded+"\n";
		} catch(DecodingFailedException e) {
			unicodeConsoleReplacement += "Provided string is not a valid base4k-encoding!\n";
		}
		
		JOptionPane.showMessageDialog(null,unicodeConsoleReplacement);
	}
}
