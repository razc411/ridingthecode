using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Graphics_Exercise2_Lab2
{
    public partial class Options : Form
    {
        public Options()
        {
            InitializeComponent();
        }

        private void left_OnClick(object sender, EventArgs e)
        {
            Program.sq.SetSquare(1);
        }

        private void Right_OnClick(object sender, EventArgs e)
        {
            Program.sq.SetSquare(0);
        }
    }
}
