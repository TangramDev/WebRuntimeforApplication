﻿using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using Universe;

namespace Sunny
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();this.Dock = DockStyle.Fill;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            string xml = "<content1>" +
                "<layout>" +
                    "<grid objid=\"TabbedWnd\" style=\"13\">" +
                        "<grid id='tangramtabctrl_Page2 www' gridtype=\"nucleus\" caption='Page 1'></grid>" +
                        "<grid id='test1' caption='Page 2 eee'></grid>" +
                        "<grid id='test2' caption='Page 3'></grid>" +
                        "<grid id='test3' caption='Page 4'></grid>" +
                    "</grid>" +
                "</layout>" +
              "</content1>";
            Grid node = Cosmos.Observe(listView1, "test", xml);
            node.OnTabChange += Node_OnTabChange1; node.OnIPCMessageReceived += Node_OnIPCMessageReceived; ;
        }

        private void Node_OnIPCMessageReceived(string strFrom, string strTo, string strMsgId, string strPayload, string strExtra)
        {
            throw new NotImplementedException();
        }

        private void Node_OnTabChange1(Grid ActiveGrid, Grid OldGrid)
        {
            //throw new NotImplementedException();
            string s1 = ActiveGrid.Caption;
            string s2 = OldGrid.Caption;
            ActiveGrid.Caption = s1 + "test";
        }
    }
}
