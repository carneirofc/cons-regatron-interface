#pragma once

#include "log/Logger.hpp"
#include "serialiolib.h" // NOLINT
#include <sstream>

namespace Regatron {

constexpr unsigned int SYS_VALUES = 64;
constexpr unsigned int MOD_VALUES = 0;

namespace State {
constexpr unsigned int POWERUP = 2;
constexpr unsigned int READY   = 4;
constexpr unsigned int RUN     = 8;
constexpr unsigned int WARNING = 10;
constexpr unsigned int ERROR   = 12;
constexpr unsigned int STOP    = 14;
} // namespace State

namespace ControlMode {
/**
0: no controller selected (output voltage is disabled)
1: constant voltage
2: constant current
4: constant power
*/
constexpr unsigned int OUTPUT_VOLTAGE_OFF = 0;
constexpr unsigned int CONST_VOLTAGE      = 0;
constexpr unsigned int CONST_CURRENT      = 0;
constexpr unsigned int CONST_POWER        = 0;
} // namespace ControlMode

namespace RemoteCtrlInp {
/*
   0: set active interface to analog
   1: set active interface to HMI/HME
   2: set active interface to RS232 (this API)
   32767: no active interface will be selected
*/

}
class Readings {
  private:
    /** One time readings */
    // Additional
    int m_DCLinkPhysNom;         // [V]
    int m_PrimaryCurrentPhysNom; // [A]
    int m_TemperaturePhysNom;    // [°C]

    // System
    double m_SysVoltagePhysMax;    // [V]
    double m_SysCurrentPhysMax;    // [A]
    double m_SysPowerPhysMax;      // [kW]
    double m_SysResistancePhysMax; // [mOhm]

    double m_SysVoltagePhysMin;    // [V]
    double m_SysCurrentPhysMin;    // [A]
    double m_SysPowerPhysMin;      // [kW]
    double m_SysResistancePhysMin; // [mOhm]

    double m_SysVoltagePhysNom;    // [V]
    double m_SysCurrentPhysNom;    // [A]
    double m_SysPowerPhysNom;      // [kW]
    double m_SysResistancePhysNom; // [mOhm]

    // -- Module
    double m_ModVoltagePhysMax;    // [V]
    double m_ModCurrentPhysMax;    // [A]
    double m_ModPowerPhysMax;      // [kW]
    double m_ModResistancePhysMax; // [mOhm]

    double m_ModVoltagePhysMin;    // [V]
    double m_ModCurrentPhysMin;    // [A]
    double m_ModPowerPhysMin;      // [kW]
    double m_ModResistancePhysMin; // [mOhm]

    double m_ModVoltagePhysNom;    // [V]
    double m_ModCurrentPhysNom;    // [A]
    double m_ModPowerPhysNom;      // [kW]
    double m_ModResistancePhysNom; // [mOhm]

  public:
    /** Monitor readings */

    // Generic ...
    unsigned int m_ControlMode;       // namespace ControlMode
    unsigned int m_RemoteCtrlInp;     // namespace Remote Ctrl Inp
    double       m_DCLinkVoltageMon;  // [V]
    double       m_PrimaryCurrentMon; // [A] Tranformer primary current
    double       m_IGBTTempMon;       // [°C] heat sink of IGBT
    double       m_RectifierTempMon;  // [°C] heat sink of rectifier
    double       m_PCBTempMon;        // [°C] PCB Controller board temperature

    // System
    struct T_ErrorTree32  m_SysErrorTree32Mon;
    struct T_ErrorTree32  m_SysWarningTree32Mon;
    constexpr static int  errorTree32Len = 32;

    unsigned int m_SysState;
    double       m_SysActualOutVoltageMon;
    double       m_SysActualOutPowerMon;
    double       m_SysActualOutCurrentMon;
    double       m_SysActualResMon;

    // Module
    struct T_ErrorTree32  m_ModErrorTree32Mon;
    struct T_ErrorTree32  m_ModWarningTree32Mon;
    unsigned int          m_ModState;
    double                m_ModActualOutVoltageMon;
    double                m_ModActualOutPowerMon;
    double                m_ModActualOutCurrentMon;
    double                m_ModActualResMon;

    // -- Regatron Module
    unsigned int m_moduleID = 0;

    /** Represent an T_ErrorTree32 object as a vector "[0, .... 32]"*/
    const std::string
    T_ErrorTree32toString(const T_ErrorTree32 &errorTree32) const {
        std::ostringstream oss;
        oss << "[" << errorTree32.group << ",";
        int index = 0;
        for (const auto &error : errorTree32.error) {
            oss << error;
            index++;
            if (index < errorTree32Len)
                oss << ",";
        }
        oss << "]";
        return oss.str();
    };

    const std::string getModTree() {
        std::ostringstream oss;
        readModuleErrorTree32();

        // Error
        oss << '[' << m_ModErrorTree32Mon.group << ',';
        for (const auto &error : m_ModErrorTree32Mon.error) {
            oss << error;
            oss << ',';
        }

        // Warning
        int idx{0};
        oss << m_ModWarningTree32Mon.group << ',';
        for (const auto &error : m_ModWarningTree32Mon.error) {
            oss << error;
            idx++;
            if (idx < errorTree32Len) {
                oss << ',';
            }
        }
        oss << ']';
        return oss.str();
    }

    const std::string getSysTree() {
        std::ostringstream oss;
        readSystemErrorTree32();

        // Error
        oss << '[' << m_SysErrorTree32Mon.group << ',';
        for (const auto &error : m_SysErrorTree32Mon.error) {
            oss << error;
            oss << ',';
        }

        // Warning
        int idx{0};
        oss << m_SysWarningTree32Mon.group << ',';
        for (const auto &error : m_SysWarningTree32Mon.error) {
            oss << error;
            idx++;
            if (idx < errorTree32Len) {
                oss << ',';
            }
        }
        oss << ']';
        return oss.str();
    }

    const std::string getModReadings() {
        readModule();
        std::ostringstream oss;
        oss << '[' << m_ModActualOutVoltageMon << ','
            << m_ModActualOutCurrentMon << ',' << m_ModActualOutPowerMon << ','
            << m_ModActualResMon << ',' << m_ModState << ']';
        return oss.str();
    }

    const std::string getSysReadings() {
        readSystem();
        std::ostringstream oss;
        oss << '[' << m_SysActualOutVoltageMon << ','
            << m_SysActualOutCurrentMon << ',' << m_SysActualOutPowerMon << ','
            << m_SysActualResMon << ',' << m_SysState << ']';
        return oss.str();
    }

    /**
     * Read and convert to physical value DCLinkVoltage
     * @throw std::runtime_exception when dll fails
     */
    void readDCLinkVoltage() {
        int DCLinkVoltStd;

        if (TC4GetDCLinkDigital(&DCLinkVoltStd) != DLL_SUCCESS) {
            throw std::runtime_error("failed to read DCLink digital voltage.");
        }
        m_DCLinkVoltageMon = (DCLinkVoltStd * m_DCLinkPhysNom) / 4000.;
    }

    void readPrimaryCurrent() {
        int primaryCurrent;
        if (TC4GetIPrimDigital(&primaryCurrent) != DLL_SUCCESS) {
            throw std::runtime_error(
                "failed to read transformer primary current.");
        }
        m_PrimaryCurrentMon =
            (primaryCurrent * m_PrimaryCurrentPhysNom) / 4000.;
    }

    /**
     * Read and convert IGBT, Rectifier and PCB temperatures.
     * @throw std::runtime_error
     */
    void readTemperature() {
        int igbtTemp, rectTemp;
        if (TC4GetTempDigital(&igbtTemp, &rectTemp) != DLL_SUCCESS) {
            throw std::runtime_error(
                "failed to read IGBT and Rectifier temperature.");
        }
        if (TC42GetTemperaturePCB(&m_PCBTempMon) != DLL_SUCCESS) {
            throw std::runtime_error("failed to read PCB temperature.");
        }
        m_IGBTTempMon      = (igbtTemp * m_TemperaturePhysNom) / 4000.;
        m_RectifierTempMon = (rectTemp * m_TemperaturePhysNom) / 4000.;
    }

    /**
     * Get a string array representation of all temperatures.
     * [0] IGBT Temp
     * [1] Rect Temp
     * [2] PCB  Temp
     * @throws: std::runtime_error
     * @return string in the format "[val1,...,valn]" */
    const std::string getTemperatures() {
        readTemperature();
        std::ostringstream oss;
        oss << '[' << m_IGBTTempMon << ',' << m_RectifierTempMon << ','
            << m_PCBTempMon << ']';
        return oss.str();
    }

    // @todo: Restrict read/write if master ...? Here or upper layer?
    bool isMaster();

    /** One time readings - @throw std::runtime_exception */
    void readAdditionalPhys();
    void readSystemPhys();
    void readModulePhys();
    void readModuleID();

    /** Monitor Readings @throw: std::runtime_exception */
    // void readGeneric();

    /** Methods readSystem and readModule will act on the following values:
     * TC4GetVoltageActSense(&m_...ActualOutVoltageMon)
     * TC4GetPowerActSense(&m_...ActualOutPowerMon)
     * TC4GetCurrentAct(&m_...ActualOutCurrentMon)
     * TC4GetResistanceAct(&m_...ActualResMon)
     * TC4StateActSystem(&m_...State)
     */
    void readSystem();
    void readModule();

    /** Methods readSystemError32 and readModuleError32 will act on the
     * following values:
     * TC4ReadErrorTree32(&m_...ErrorTree32Mon)
     * TC4ReadWarningTree32(&m_...WarningTree32Mon)
     * */
    void readSystemErrorTree32();
    void readModuleErrorTree32();

    /** Set Module/System calls */
    void selectSystem() { this->selectModule(SYS_VALUES); }
    void selectModule() { this->selectModule(MOD_VALUES); }

    void storeParameters() {
        if (TC4StoreParameters() != DLL_SUCCESS)
            throw std::runtime_error("failed to store parameters");
    }

    void clearErrors() {
        if (TC4ClearError() != DLL_SUCCESS)
            throw std::runtime_error("failed to clear erors");
    }

    void readControlMode() {
        if (TC4GetControlMode(&m_ControlMode) != DLL_SUCCESS)
            throw std::runtime_error("failed to read control mode");
    }

    void readRemoteControlInput() {
        if (TC4GetRemoteControlInput(&m_RemoteCtrlInp) != DLL_SUCCESS)
            throw std::runtime_error("failed to read remote control input");
    }

    void selectModule(unsigned int module) {
        if (TC4SetModuleSelector(module) != DLL_SUCCESS)
            throw std::runtime_error(fmt::format(
                "failed to set module selector to {} (code {})",
                ((module == SYS_VALUES) ? "system" : "device"), module));
    }
};
} // namespace Regatron
