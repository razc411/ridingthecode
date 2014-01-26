using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Drawing.Drawing2D;

namespace Graphics_Exercise1_Lab2
{
    /*
     * I didn't comment because I didn't think it was nessecary for a simple program like these.
     */
    public partial class Circles : Form
    {
         public Circles()
        {
            Text = "C4560: Assignment 2, Exercise 1, A00825005/Ramzi Chennafi/4O";
            BackColor = Color.Black;
            ResizeRedraw = true;
            SetStyle(ControlStyles.UserPaint, true);
            SetStyle(ControlStyles.AllPaintingInWmPaint, true);
            SetStyle(ControlStyles.DoubleBuffer, true);
        }

        protected override void OnPaint(PaintEventArgs pea)
        {
            Graphics gfx = pea.Graphics;

            gfx.SmoothingMode = SmoothingMode.HighQuality;
            gfx.PixelOffsetMode = PixelOffsetMode.HighQuality;

            int cx = ClientSize.Width;
            int cy = ClientSize.Height;
            int s = (cx > cy) ? cy : cx;
            int Mx = cx / 2;
            int My = cy / 2;
            int r = s / 12;
            int R = 2 * r;
            
            int[] MxCircles = {(Mx - (5 * r)), (Mx - (5 * r)), (Mx + (1 * r)), (Mx + (1*r))}; // large circles
            int[] MyCircles = {(My - (5 * r)), (My + (1 * r)), (My - (5 * r)), (My + (1*r))};
            int[] MxSCircles = {(Mx - (4 * r)), (Mx - (4 * r)), (Mx + (2 * r)), (Mx + (2*r))}; // small circles
            int[] MySCircles = {(My - (4 * r)), (My + (2 * r)), (My - (4 * r)), (My + (2 * r)) };
           
            SolidBrush BlueBrush = new SolidBrush(Color.Blue);
            SolidBrush GreenBrush = new SolidBrush(Color.Green);
            SolidBrush RedBrush = new SolidBrush(Color.Red);
            SolidBrush YellowBrush = new SolidBrush(Color.Yellow);

            gfx.FillEllipse(GreenBrush, MxCircles[0], MyCircles[0], 2*R, 2*R); // top left
            gfx.FillEllipse(GreenBrush, MxCircles[1], MyCircles[1], 2*R, 2*R); // bottom left
            gfx.FillEllipse(YellowBrush, MxCircles[2], MyCircles[2], 2*R, 2*R); // top right
            gfx.FillEllipse(YellowBrush, MxCircles[3], MyCircles[3], 2*R, 2*R); // bottom right
            gfx.FillEllipse(BlueBrush, MxSCircles[0], MySCircles[0], R, R); // top left
            gfx.FillEllipse(RedBrush, MxSCircles[1], MySCircles[1], R, R); // bottom left
            gfx.FillEllipse(BlueBrush, MxSCircles[2], MySCircles[2], R, R); // top right
            gfx.FillEllipse(RedBrush, MxSCircles[3], MySCircles[3], R, R); //bottom right
            
        }
    }
}
