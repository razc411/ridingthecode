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

namespace Graphics_Exercise2_Lab2
{
    public partial class Squares : Form
    {
        int side = 0;
        public Squares()
        {
            Text = "C4560: Assignment 2, Exercise 2, A00825005/Ramzi Chennafi/4O";
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

            int g = 15;
            int s = 5;

            int Mx = cx / 2;
            int My = (s + g);

            int numWSquares = (cx - (2 * g) - s) / (s + g);
            int numHSquares = (cy - (2 * g) - s) / (s + g);

            int sqXPosLeft = Mx - g;
            int sqXPosRight = Mx;
            int sqYPos = s + g;

            SolidBrush brush = new SolidBrush(Color.SpringGreen);
            SolidBrush WhiteBrush = new SolidBrush(Color.White);

            if (side == 1)
                gfx.FillRectangle(WhiteBrush, 0, 0, cx / 2, cy);
            else
            {
                gfx.FillRectangle(WhiteBrush, cx / 2, 0, cx / 2, cy);
            }

            for (int i = 0; i < numHSquares; i++)
            
            {
                for (int j = 0; j < (numWSquares / 2); j++)
                {
                    gfx.FillRectangle(brush, sqXPosLeft, sqYPos, g, g);
                    gfx.FillRectangle(brush, sqXPosRight, sqYPos, g, g);
                    sqXPosLeft -= (s + g);
                    sqXPosRight += (s + g);
                }
                sqXPosLeft = Mx - g;
                sqXPosRight = Mx;
                sqYPos += s + g;
            }
        }

        public void SetSquare(int clickside){
            side = clickside;
            this.Refresh();
        }
    }
}
