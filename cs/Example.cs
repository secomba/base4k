using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using Secomba;

namespace Example
{
    class Example
    {
        static void Main(string[] args)
        {
            Encoding enc = new UTF8Encoding(true, true);
            string consoleRepl = "";
            
            string plain = "please encode me!";

            consoleRepl += "## plaintext ##\n";
            consoleRepl += plain + "\n";
		
		    string encoded = Base4K.Encode(enc.GetBytes(plain));
            consoleRepl += "## encoded ##\n";
            consoleRepl += encoded + "\n";
            consoleRepl += "## decoded ##\n";
		    string decoded = enc.GetString(Base4K.Decode(encoded));
            if(decoded!=null)
                consoleRepl += decoded+"\n";
            else
                consoleRepl += "Provided string is not a valid base4k-encoding!\n";

            MessageBox.Show(consoleRepl);
        }
    }
}
