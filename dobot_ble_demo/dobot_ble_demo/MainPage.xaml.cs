using System; 
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Data;
using System.Text;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using Windows.Devices.Bluetooth;
using Windows.Devices.Bluetooth.GenericAttributeProfile;
using Windows.Devices.Bluetooth.Advertisement;
using Windows.Devices.Enumeration;
using Windows.Storage.Streams;
using Windows.Security.Cryptography;
using Windows.UI.Popups;


// Pour plus d'informations sur le modèle d'élément Page vierge, consultez la page https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace DobotBLEDemo
{
    /// <summary>
    /// Une page vide peut être utilisée seule ou constituer une page de destination au sein d'un frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        public MainPage()
        {
            this.InitializeComponent();

            watcher = new BluetoothLEAdvertisementWatcher();
            watcher.Received += OnAdvertisementReceived;
            watcher.Start();

            StartDobot();

        }


        BluetoothLEAdvertisementWatcher watcher = null;
        BluetoothLEDevice myDevice = null;
        GattCharacteristic myCharacteristic = null;
        DispatcherTimer dispatcherTimer;

        private byte isJoint = (byte)0;
        private bool isConnectted = false;
        private JogCmd currentCmd;
        private System.Timers.Timer posTimer = new System.Timers.Timer();

        float oldXOrientation = 0;
        float oldYOrientation = 0;
        float oldZOrientation = 0;

        static ulong RSL10_BluetoothAddress = 106380957178854;
        static String RSL10_Motion_BLE_SERVICE_UIID = "69c482ca-c200-44fc-b048-6e0d0be1191c";
        static String RSL10_Motion_BLE_CHARACTERISTIC_UIID = "69c482ca-c200-44fc-b048-6e0d0be1191d";
        static String Dobot_IP_ADDRESS = "192.168.43.84";

        private async void OnAdvertisementReceived(BluetoothLEAdvertisementWatcher watcher, BluetoothLEAdvertisementReceivedEventArgs eventArgs)
        {

            if ((eventArgs.BluetoothAddress != RSL10_BluetoothAddress) || (myDevice != null))
                return;
            BluetoothLEDevice x = await BluetoothLEDevice.FromBluetoothAddressAsync(eventArgs.BluetoothAddress);
            try
            {
                myDevice = x;
                button1.IsEnabled = true;
                watcher.Stop();
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
        }

        private async void Button_Click(object sender, RoutedEventArgs e)
        {
            if (myDevice == null)
                return;
            if (myCharacteristic == null)
            {
                GattDeviceServicesResult result = await myDevice.GetGattServicesAsync();
                foreach (var service in result.Services)
                {
                    if (service.Uuid.ToString() == RSL10_Motion_BLE_SERVICE_UIID)
                    {
                        GattCharacteristicsResult CharResult = await service.GetCharacteristicsAsync(BluetoothCacheMode.Uncached);
                        foreach (var characteristic in CharResult.Characteristics)
                        {
                            if (characteristic.Uuid.ToString() == RSL10_Motion_BLE_CHARACTERISTIC_UIID)
                            {
                                myCharacteristic = characteristic;
                                dispatcherTimer = new DispatcherTimer();
                                dispatcherTimer.Tick += dispatcherTimer_Tick;
                                dispatcherTimer.Interval = new TimeSpan(0, 0, 0, 0, 100);//100 ms
                                dispatcherTimer.Start();

                            }

                        }
                    }
                }

            }

        }
        async void dispatcherTimer_Tick(object sender, object e)
        {
            UInt64 cmdIndex = 0;
            GattReadResult result = await myCharacteristic.ReadValueAsync(BluetoothCacheMode.Uncached);
            if (result.Status == GattCommunicationStatus.Success)
            {
                var reader = DataReader.FromBuffer(result.Value);
                byte[] input = new byte[reader.UnconsumedBufferLength];
                reader.ReadBytes(input);
                float XOrientation = BitConverter.ToSingle(new Byte[] { input[0], input[1], input[2], input[3] }, 0);
                float YOrientation = BitConverter.ToSingle(new Byte[] { input[4], input[5], input[6], input[7] }, 0);
                float ZOrientation = BitConverter.ToSingle(new Byte[] { input[8], input[9], input[10], input[11] }, 0);
                float DX = XOrientation - oldXOrientation;
                oldXOrientation = XOrientation;
                if (DX>1)
                {
                    currentCmd.isJoint = isJoint;
                    currentCmd.cmd = (byte)JogCmdType.JogAPPressed;
                    DobotDll.SetJOGCmd(ref currentCmd, false, ref cmdIndex);
                }
                else if (DX < -1)
                {
                    currentCmd.isJoint = isJoint;
                    currentCmd.cmd = (byte)JogCmdType.JogANPressed;
                    DobotDll.SetJOGCmd(ref currentCmd, false, ref cmdIndex);
                }

                float DY = YOrientation - oldYOrientation;
                oldYOrientation = YOrientation;
                if (DY > 1)
                {
                    currentCmd.isJoint = isJoint;
                    currentCmd.cmd = (byte)JogCmdType.JogBPPressed;
                    DobotDll.SetJOGCmd(ref currentCmd, false, ref cmdIndex);
                }
                else if (DY < -1)
                {
                    currentCmd.isJoint = isJoint;
                    currentCmd.cmd = (byte)JogCmdType.JogBNPressed;
                    DobotDll.SetJOGCmd(ref currentCmd, false, ref cmdIndex);
                }

                float DZ = ZOrientation - oldZOrientation;
                oldZOrientation = ZOrientation;
                if (DZ > 1)
                {
                    currentCmd.isJoint = isJoint;
                    currentCmd.cmd = (byte)JogCmdType.JogCPPressed;
                    DobotDll.SetJOGCmd(ref currentCmd, false, ref cmdIndex);
                }
                else if (DZ < -1)
                {
                    currentCmd.isJoint = isJoint;
                    currentCmd.cmd = (byte)JogCmdType.JogCNPressed;
                    DobotDll.SetJOGCmd(ref currentCmd, false, ref cmdIndex);
                }

                String s = String.Format("X={0} ; Y={1} ; Z={2}", XOrientation, YOrientation, ZOrientation);

                textBlock.Text = s;
            }
        }


        /// <summary>
        /// StartDobot
        /// </summary>
        private async void StartDobot()
        {
            StringBuilder fwType = new StringBuilder(60);
            StringBuilder version = new StringBuilder(60);

            int ret = DobotDll.ConnectDobot(Dobot_IP_ADDRESS, 115200, fwType, version);
            
            // start connect
            if (ret != (int)DobotConnect.DobotConnect_NoError)
            {
                var messageDialogError = new MessageDialog("Connect Error");
                await messageDialogError.ShowAsync();
                return;
            }

            isConnectted = true;
            DobotDll.SetCmdTimeout(3000);

            // Get name
            string deviceName = "Dobot Magician";
            DobotDll.SetDeviceName(deviceName);
            StringBuilder deviceSN = new StringBuilder(64);
            DobotDll.GetDeviceName(deviceSN, 64);

            SetParam();
        }

        private void SetParam()
        {
            UInt64 cmdIndex = 0;
            JOGJointParams jsParam;
            jsParam.velocity = new float[] { 200, 200, 200, 200 };
            jsParam.acceleration = new float[] { 200, 200, 200, 200 };
            DobotDll.SetJOGJointParams(ref jsParam, false, ref cmdIndex);

            JOGCommonParams jdParam;
            jdParam.velocityRatio = 100;
            jdParam.accelerationRatio = 100;
            DobotDll.SetJOGCommonParams(ref jdParam, false, ref cmdIndex);

            PTPJointParams pbsParam;
            pbsParam.velocity = new float[] { 200, 200, 200, 200 };
            pbsParam.acceleration = new float[] { 200, 200, 200, 200 };
            DobotDll.SetPTPJointParams(ref pbsParam, false, ref cmdIndex);

            PTPCoordinateParams cpbsParam;
            cpbsParam.xyzVelocity = 100;
            cpbsParam.xyzAcceleration = 100;
            cpbsParam.rVelocity = 100;
            cpbsParam.rAcceleration = 100;
            DobotDll.SetPTPCoordinateParams(ref cpbsParam, false, ref cmdIndex);

            PTPJumpParams pjp;
            pjp.jumpHeight = 20;
            pjp.zLimit = 100;
            DobotDll.SetPTPJumpParams(ref pjp, false, ref cmdIndex);

            PTPCommonParams pbdParam;
            pbdParam.velocityRatio = 30;
            pbdParam.accelerationRatio = 30;
            DobotDll.SetPTPCommonParams(ref pbdParam, false, ref cmdIndex);


            PTPLParams pTPLParams;
            pTPLParams.velocity = 50;
            pTPLParams.acceleration = 50;
            DobotDll.SetPTPLParams(ref pTPLParams, false, ref cmdIndex);

            JogLParams joglparams;
            joglparams.velocity = 50;
            joglparams.acceleration = 50;
            DobotDll.SetJOGLParams(ref joglparams, false, ref cmdIndex);


        }


        // control event handle
        private void OnEvent(object sender)
        {
            if (!isConnectted)
                return;

            UInt64 cmdIndex = 0;
            Button obj = (Button)sender;
            String con = obj.Content.ToString();
            switch (con)
            {

                case "R+":
                case "Joint4+":
                    {
                        currentCmd.isJoint = isJoint;
                        currentCmd.cmd = (byte)JogCmdType.JogDPPressed;
                        DobotDll.SetJOGCmd(ref currentCmd, false, ref cmdIndex);
                    }
                    break;
                case "R-":
                case "Joint4-":
                    {
                        currentCmd.isJoint = isJoint;
                        currentCmd.cmd = (byte)JogCmdType.JogDNPressed;
                        DobotDll.SetJOGCmd(ref currentCmd, false, ref cmdIndex);
                    }
                    break;
                default:
                    break;
            }
        }
    }
}
