using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Drawing;

namespace Graphics_Exercise2_Lab2
{
    static class Program
    {
        public static Squares sq;
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Options toggle = new Options();
            toggle.Show();
            sq = new Squares();
            Application.Run(sq);
        }
    }
}
