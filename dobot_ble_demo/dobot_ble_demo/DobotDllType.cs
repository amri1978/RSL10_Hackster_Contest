using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace DobotBLEDemo
{
    

    public enum EndType {
        EndTypeCustom,
        EndTypeSuctionCap,
        EndTypeGripper,
        EndTypeLaser,
        EndTypePen,
        EndTypeMax,
        EndTypeNum = EndTypeMax
    };

    [StructLayout(LayoutKind.Sequential, CharSet=CharSet.Ansi, Pack=1)]
    public struct EndTypeParams {
         public float xBias;
         public float yBias;
         public float zBias;
    };

    public struct Pose {
        public float x;
        public float y;
        public float z;
     
        public float rHead;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
        public float[] jointAngle;
    };


    public struct Kinematics {
        public float velocity;
        public float acceleration;
    };


    public struct AlarmsState {
        public int alarmsState;
    };

    /*********************************************************************************************************
    ** HOME
    *********************************************************************************************************/
    public struct HOMEParams {
        public float x;
        public float y;
        public float z;
        public float r;
    };

    public struct HOMECmd {
        public int temp;
    };

    public struct JOGJointParams
    {
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
        public float[] velocity;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
        public float[] acceleration;
    };

    public struct JOGCoordinateParams
    {
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
        public float[] velocity;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
        public float[] acceleration;
    };



    public  struct JogLParams
    {
        public float velocity; 
        public  float acceleration; 
    };
    

    public struct JOGCommonParams
    {
        public float velocityRatio;
        public float accelerationRatio;
    };

    /*
     * Jog Cmd
     */
    public enum JogCmdType{
        JogIdle,
        JogAPPressed,
        JogANPressed,
        JogBPPressed,
        JogBNPressed,
        JogCPPressed,
        JogCNPressed,
        JogDPPressed,
        JogDNPressed,
        JogEPPressed,
        JogENPressed
    };

    /*
     * Jog instant cmd
     */
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi, Pack = 1)]
    public struct JogCmd {
        public byte isJoint;
        public byte cmd;
    };

    public struct PTPJointParams
    {
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
        public float[] velocity;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
        public float[] acceleration;
    };
    public struct PTPCoordinateParams
    {
        public float xyzVelocity;
        public float rVelocity;
        public float xyzAcceleration;
        public float rAcceleration;
    };

    public struct PTPJumpParams
    {
        public float jumpHeight;
        public float zLimit;
    };

    public struct PTPCommonParams
    {
        public float velocityRatio;
        public float accelerationRatio;
    };

    // For play back
    public enum PTPMode {
        PTPJUMPXYZMode,
        PTPMOVJXYZMode,
        PTPMOVLXYZMode,

        PTPJUMPANGLEMode,
        PTPMOVJANGLEMode,
        PTPMOVLANGLEMode,

        PTPMOVJXYZINCMode,
        PTPMOVLXYZINCMode,
    };

    public struct PTPLParams
    {
        public float velocity; 
        public float acceleration;
    };
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi, Pack = 1)]
    public struct PTPWithLCmd
    {
        public byte ptpMode; //PTP：0~9
        public float x; //（x,y,z,r
        public float y;
        public float z;
        public float rHead;
        public float l; 
    };


    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi, Pack = 1)]
    public struct PTPCmd
    {
        public byte ptpMode;
        public float x;
        public float y;
        public float z;
        public float rHead;
    };

    /*********************************************************************************************************
    ** Continuous path
    *********************************************************************************************************/
    /*
     * CP
     */
    public struct CPParams
    {
        public float planAcc;
        public float juncitionVel;
        public float acc;
        public byte realTimeTrack;
    };

    public enum ContinuousPathMode {
        CPRelativeMode,
        CPAbsoluteMode
    };
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi, Pack = 1)]
    public struct CPCmd
    {
        public byte cpMode;
        public float x;
        public float y;
        public float z;
        public float velocity;
    };

    /*********************************************************************************************************
    ** User parameters
    *********************************************************************************************************/

    public struct WAITCmd
    {
        public UInt32 timeout;
    };

    public enum IOFunction
    {
        IOFunctionDummy,
        IOFunctionDO,
        IOFunctionPWM,
        IOFunctionDI,
        IOFunctionADC
    };

    public struct  IOMultiplexing{
        public byte address;
        public byte multiplex;
    };

    public struct IODO{
        public byte  address;
        public byte  level;
    };

    public struct IOPWM {
        public byte address;
        public float frequency;
        public float dutyCycle;
    };

    public struct IODI {
        public byte address;
        public byte level;
    };

    public struct IOADC
    {
        public byte address;
        public UInt16 value;
    };

    /*
     * ARC related
     */
    public struct ARCParams
    {
        public float xyzVelocity;
        public float rVelocity;
        public float xyzAcceleration;
        public float rAcceleration;
    };

    public struct ARCCmd {
        public float cirPoint_x;
        public float cirPoint_y;
        public float cirPoint_z;
        public float cirPoint_r;

        public float toPoint_x;
        public float toPoint_y;
        public float toPoint_z;
        public float toPoint_r;
    };

    /*********************************************************************************************************
    ** User parameters
    *********************************************************************************************************/
    public struct UserParams {
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 8)]
        public float[] param;
    };

    /*********************************************************************************************************
    ** API result
    *********************************************************************************************************/
    public enum DobotConnect{
        DobotConnect_NoError,
        DobotConnect_NotFound,
        DobotConnect_Occupied
    };

    public enum DobotCommunicate{
        DobotCommunicate_NoError,
        DobotCommunicate_BufferFull,
        DobotCommunicate_Timeout,
        DobotCommunicate_InvalidParams
    };
}
