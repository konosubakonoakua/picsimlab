/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2010-2023  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   For e-mail suggestions :  lcgamboa@yahoo.com
   ######################################################################## */

#ifndef PICSIMLAB
#define PICSIMLAB

#define BASETIMER 100                 // timer period in ms
#define NSTEPKF (4000.0 / BASETIMER)  // Freq constant 4.0*timer_freq
#define NSTEPKT (1e6 / NSTEPKF)       // TIMER constant 1MHz/(4.0*timer_freq)
#define DEFAULTJS 100                 // IO refresh rate

extern char SERIALDEVICE[100];

#define MAX_MIC 140

#include "board.h"
#include "util.h"

enum PICSimlabCPUState { CPU_RUNNING, CPU_STEPPING, CPU_HALTED, CPU_BREAKPOINT, CPU_ERROR, CPU_POWER_OFF, CPU_LAST };

enum PICSimlabStatus { PS_RUN = 0, PS_DEBUG, PS_SERIAL, PS_LAST };

class CPICSimLab {
public:
    CPICSimLab();

    void Init(CWindow* w);

    CWindow* GetWindow(void) { return Window; };

    /**
     * @brief  Get the file path of resources
     */
    std::string GetSharePath(void) { return SHARE; };
    void SetSharePath(std::string spath) { SHARE = spath; };

    std::string GetLibPath(void) { return libpath; };
    void SetLibPath(std::string lpath) { libpath = lpath; };

    std::string GetHomePath(void) { return HOME; };
    void SetHomePath(std::string home) { HOME = home; };

    std::string GetPath(void) { return PATH; };
    void SetPath(std::string path) { PATH = path; };

    std::string GetFNAME(void) { return FNAME; };
    void SetFNAME(std::string fname) { FNAME = fname; };

    std::string GetOldPath(void) { return OldPath; };
    void SetOldPath(std::string op) { OldPath = op; };

    std::string GetProcessorName(void) { return proc_; };
    void SetProcessorName(std::string pn) { proc_ = pn; };

    long int GetNSTEP(void) { return NSTEP; };
    void SetNSTEP(long int ns) { NSTEP = ns; };

    /**
     * @brief  Get the number of steps in 100ms of simulation
     */
    long int GetNSTEPJ(void) { return NSTEPJ; };
    void SetNSTEPJ(long int nsj) { NSTEPJ = nsj; };

    /**
     * @brief  Get the number of steps to be skipped in board update
     */
    int GetJUMPSTEPS(void) { return JUMPSTEPS; };
    void SetJUMPSTEPS(int js);

    /**
     * @brief  Return a pointer to board object
     */
    board* GetBoard(void) { return pboard; };
    void SetBoard(board* b) { pboard = b; };
    void DeleteBoard(void);

    /**
     * @brief  Return actual power status of microcontroller ON/OFF
     */
    int GetMcuPwr(void) { return mcupwr; };

    /**
     * @brief  Retunr if microcontroller reset pin is enabled
     */
    int GetMcuRst(void) { return mcurst; };

    int GetMcuDbg(void) { return mcudbg; };

    /**
     * @brief  Set the power status of microcontroller ON/OFF
     */
    void SetMcuPwr(int pp) {
        mcupwr = pp;
        if (mcupwr)
            SetCpuState(CPU_RUNNING);
        else
            SetCpuState(CPU_POWER_OFF);
    };

    void SetCpuState(const PICSimlabCPUState cs);

    /**
     * @brief  Set mcu rst flag (inform simulator about mcu reset state)
     */
    void SetMcuRst(int pr) { mcurst = pr; };

    char GetCpuState(void) { return cpustate; };

    void Set_mcudbg(int pd);

    void SetMcuRun(int mr) { mcurun = mr; };
    int GetMcuRun(void) { return mcurst; };

    /**
     * @brief  Return the selected debugger port
     */
    unsigned short GetDebugPort(void) { return debug_port + Instance; };

    /**
     * @brief  Set debug TCP port
     */
    void SetDebugPort(unsigned short dp);

    /**
     * @brief  Return the selected remote control port
     */
    unsigned short GetRemotecPort(void) { return remotec_port + Instance; };

    /**
     * @brief  Set remote control TCP port
     */
    void SetRemotecPort(unsigned short rcp);

    /**
     * @brief  Return the program instance number
     */
    int GetInstanceNumber(void) { return Instance; };

    /**
     * @brief  Return the selected debugger type
     */
    int GetDebugType(void) { return debug_type; };

    /**
     * @brief  Set debug type (MDB or GDB)
     */
    void SetDebugType(int dt) { debug_type = dt; };

    /**
     * @brief  Get debug status (On/Off)
     */
    int GetDebugStatus(void);

    /**
     * @brief  Set debug status flag
     */
    void SetDebugStatus(int dbs, int updatebtn = 1);

    void SetClock(const float clk, const int update = 1);
    float GetClock(void);

    int GetNeedClkUpdate(void) { return need_clkupdate; };

    /**
     * @brief  Save the preferences
     */
    void SavePrefs(std::string name, std::string value);

    void PrefsClear(void) { prefs.clear(); };
    int PrefsSaveToFile(std::string fname) { return SaveToFile(prefs, fname.c_str()); };
    int PrefsLoadFromFile(std::string fname) { return LoadFromFile(prefs, fname.c_str()); };
    unsigned int PrefsGetLinesCount(void) { return prefs.size(); };
    std::string PrefsGetLine(int ln) { return prefs.at(ln); };

    void OpenLoadHexFileDialog(void);

    void SetNeedReboot(int nr = 1);
    int GetNeedReboot(void) { return NeedReboot; };

    void RegisterError(const std::string error);
    int GetErrorCount(void) { return Errors.size(); };

    std::string GetError(int en) { return Errors.at(en); };

    void DeleteError(int en) { Errors.erase(Errors.begin() + en); };

    void EndSimulation(int saveold = 0, const char* newpath = NULL);

    void SetWorkspaceFileName(const std::string fname) { Workspacefn = fname; };

    void SetLabs(const int lb, const int lb_) {
        lab = lb;
        lab_ = lb_;
    };

    int GetLab(void) { return lab; };
    int GetLab_(void) { return lab_; };

    int LoadHexFile(std::string fname);

    void LoadWorkspace(std::string fnpzw, const int show_readme = 1);
    void SaveWorkspace(std::string fnpzw);

    void SetSimulationRun(int run);
    int GetSimulationRun(void);

    double GetScale(void) { return scale; };
    void SetScale(double s) { scale = s; };

    int GetNeedResize(void) { return need_resize; };
    void SetNeedResize(int nr) { need_resize = nr; };

    void Configure(const char* home, int use_default_board = 0, int create = 0, const char* lfile = NULL,
                   const int disable_debug = 0);

    void SetplWidth(int pw) { plWidth = pw; };

    void SetplHeight(int ph) { plHeight = ph; };

    double GetIdleMs(void);
    void SetIdleMs(double im);

    int GetUseDSRReset(void) { return use_dsr_reset; };
    void SetUseDSRReset(int udsr) { use_dsr_reset = udsr; };

    void SetToDestroy(void);
    int GetToDestroy(void) { return settodestroy; };

    void SetSync(unsigned char s) { sync = s; };
    unsigned char GetSync(void) { return sync; };

    char* GetPzwTmpdir(void) { return pzwtmpdir; };

    void UpdateStatus(const PICSimlabStatus field, const std::string msg);

#ifndef _NOTHREAD
    lxCondition* cpu_cond;
    lxMutex* cpu_mutex;
#endif
    union {
        char st[2];
        unsigned short int status;
    } status;

    int tgo;

    int plWidth;
    int plHeight;
    int use_dsr_reset;

    CItemMenu MBoard[BOARDS_MAX];
    CItemMenu MMicro[MAX_MIC];

    void (*updatestatus)(const int field, const std::string msg);
    void (CControl::*menu_EvBoard)(CControl* control);
    void (CControl::*menu_EvMicrocontroller)(CControl* control);
    void (CControl::*board_Event)(CControl* control);
    void (CControl::*board_ButtonEvent)(CControl* control, const uint button, const uint x, const uint y,
                                        const uint mask);

private:
    void StartRControl(void);
    CWindow* Window;
    board* pboard;
    int lab;
    int lab_;
    std::string SHARE;
    std::string libpath;
    long int NSTEP;
    long int NSTEPJ;
    int JUMPSTEPS;
    int mcurun;
    int mcupwr;
    int mcurst;
    int mcudbg;
    PICSimlabCPUState cpustate;
    unsigned short debug_port;
    unsigned short remotec_port;
    std::string HOME;
    std::string PATH;
    std::string FNAME;
    std::string OldPath;
    std::string proc_;
    std::string pzw_ver;
    int Instance;
    int debug_type;
    int debug;
    int need_resize;
    int need_clkupdate;
    std::vector<std::string> prefs;
    int NeedReboot;
    std::vector<std::string> Errors;
    std::string Workspacefn;
    double scale;
    double idle_ms;
    int settodestroy;
    unsigned char sync;
    char pzwtmpdir[1024];
};

extern CPICSimLab PICSimLab;

#ifdef _WIN_
#define msleep(x) Sleep(x)
// extern void usleep(unsigned int usec);
#include <unistd.h>
#else
#define msleep(x) usleep(x * 1000)
#endif

#define ST_T1 0x01
#define ST_T2 0x02
#define ST_TH 0x04
#define ST_DI 0x80  // DISABLE

#ifdef _WIN_
#define NULLFILE "\\\\.\\NUL"
#else
#define NULLFILE "/dev/null"
#endif

#endif  // PICSIMLAB
