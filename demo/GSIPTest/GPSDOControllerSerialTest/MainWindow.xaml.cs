using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace GPSDOControllerSerialTest
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        SerialPort serialPort;
        bool connectFlag = false;

        public MainWindow()
        {
            InitializeComponent();

            // load all serial ports
            string[] ports = SerialPort.GetPortNames();
            for (int i = 0; i < ports.Length; i++)
            {
                ComboBoxItem item = new ComboBoxItem();
                item.Content = ports[i];
                comboBoxPort.Items.Add(item);
            }
            comboBoxPort.SelectedIndex = 0;
        }

        private void buttonSerial_Click(object sender, RoutedEventArgs e)
        {
            if (!connectFlag)
            {
                string port = comboBoxPort.Text;
                int baudrate = int.Parse(comboBoxBaudrate.Text);

                serialPort = new SerialPort(port, baudrate);
                serialPort.Open();
            }
            else
            {
                serialPort.Close();
                serialPort.Dispose();
            }

            if (serialPort.IsOpen)
            {
                connectFlag = true;
                buttonSerial.Content = "Disconnect";
                labelStatus.Content = "Connected";
            }
            else
            {
                connectFlag = false;
                buttonSerial.Content = "Connect";
                labelStatus.Content = "Disconnected";
            }
        }
    }
}
