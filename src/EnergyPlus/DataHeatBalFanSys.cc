// EnergyPlus, Copyright (c) 1996-2019, The Board of Trustees of the University of Illinois,
// The Regents of the University of California, through Lawrence Berkeley National Laboratory
// (subject to receipt of any required approvals from the U.S. Dept. of Energy), Oak Ridge
// National Laboratory, managed by UT-Battelle, Alliance for Sustainable Energy, LLC, and other
// contributors. All rights reserved.
//
// NOTICE: This Software was developed under funding from the U.S. Department of Energy and the
// U.S. Government consequently retains certain rights. As such, the U.S. Government has been
// granted for itself and others acting on its behalf a paid-up, nonexclusive, irrevocable,
// worldwide license in the Software to reproduce, distribute copies to the public, prepare
// derivative works, and perform publicly and display publicly, and to permit others to do so.
//
// Redistribution and use in source and binary forms, with or without modification, are permitted
// provided that the following conditions are met:
//
// (1) Redistributions of source code must retain the above copyright notice, this list of
//     conditions and the following disclaimer.
//
// (2) Redistributions in binary form must reproduce the above copyright notice, this list of
//     conditions and the following disclaimer in the documentation and/or other materials
//     provided with the distribution.
//
// (3) Neither the name of the University of California, Lawrence Berkeley National Laboratory,
//     the University of Illinois, U.S. Dept. of Energy nor the names of its contributors may be
//     used to endorse or promote products derived from this software without specific prior
//     written permission.
//
// (4) Use of EnergyPlus(TM) Name. If Licensee (i) distributes the software in stand-alone form
//     without changes from the version obtained under this License, or (ii) Licensee makes a
//     reference solely to the software portion of its product, Licensee must refer to the
//     software as "EnergyPlus version X" software, where "X" is the version number Licensee
//     obtained under this License and may not use a different name for the software. Except as
//     specifically required in this Section (4), Licensee shall not use in a company name, a
//     product name, in advertising, publicity, or other promotional activities any name, trade
//     name, trademark, logo, or other designation of "EnergyPlus", "E+", "e+" or confusingly
//     similar designation, without the U.S. Department of Energy's prior written consent.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

// EnergyPlus Headers
#include <DataHeatBalFanSys.hh>
#include <DataPrecisionGlobals.hh>

namespace EnergyPlus {

namespace DataHeatBalFanSys {

    // MODULE INFORMATION:
    //       AUTHOR         Richard J. Liesen
    //       DATE WRITTEN   February 1997
    //       MODIFIED       na
    //       RE-ENGINEERED  na

    // PURPOSE OF THIS MODULE:
    // This module should contains the information that is needed to pass from the
    // Heat Balance Module to the Fan Systems

    // Using/Aliasing
    using namespace DataPrecisionGlobals;

    // Data
    // -only module should be available to other modules and routines.
    // Thus, all variables in this module must be PUBLIC.

    // MODULE PARAMETER DEFINITIONS:
    EP_GLOBAL int const UseSimpleAirFlow(1);
    EP_GLOBAL Real64 const MaxRadHeatFlux(4000.0); // [W/m2] max limit for radiant heat flux at a surface due to HVAC equipment

    // Controls for PredictorCorrector
    EP_GLOBAL int const iGetZoneSetPoints(1);
    EP_GLOBAL int const iPredictStep(2);
    EP_GLOBAL int const iCorrectStep(3);
    EP_GLOBAL int const iRevertZoneTimestepHistories(4);
    EP_GLOBAL int const iPushZoneTimestepHistories(5);
    EP_GLOBAL int const iPushSystemTimestepHistories(6);

    // DERIVED TYPE DEFINITIONS:

    // MODULE VARIABLE DECLARATIONS:
    EP_GLOBAL Array1D<Real64> SumConvHTRadSys;         // Sum of convection to zone air from hi temp radiant heaters
    EP_GLOBAL Array1D<Real64> SumLatentHTRadSys;       // Sum of latent gains from hi temp radiant heaters
    EP_GLOBAL Array1D<Real64> SumConvPool;             // Sum of convection to zone air from pools
    EP_GLOBAL Array1D<Real64> SumLatentPool;           // Sum of latent gains from pools
    EP_GLOBAL Array1D<Real64> QHTRadSysToPerson;       // Sum of radiant gains to people from hi temp radiant heaters
    EP_GLOBAL Array1D<Real64> QHWBaseboardToPerson;    // Sum of radiant gains to people from hot water baseboard heaters
    EP_GLOBAL Array1D<Real64> QSteamBaseboardToPerson; // Sum of radiant gains to people from steam baseboard heaters
    EP_GLOBAL Array1D<Real64> QElecBaseboardToPerson;  // Sum of radiant gains to people from electric baseboard heaters
    EP_GLOBAL Array1D<Real64> QCoolingPanelToPerson;   // Sum of radiant losses to people from cooling panels
    // Zone air drybulb conditions variables
    EP_GLOBAL Array1D<Real64> ZTAV;         // Zone Air Temperature Averaged over the Zone Time step
    EP_GLOBAL Array1D<Real64> MAT;          // MEAN AIR TEMPARATURE (C)
    EP_GLOBAL Array1D<Real64> TempTstatAir; // temperature of air near the thermo stat
    EP_GLOBAL Array1D<Real64> ZT;           // Zone Air Temperature Averaged over the System Time Increment
    EP_GLOBAL Array1D<Real64> XMAT;         // TEMPORARY ZONE TEMPERATURE TO TEST CONVERGENCE
    EP_GLOBAL Array1D<Real64> XM2T;
    EP_GLOBAL Array1D<Real64> XM3T;
    EP_GLOBAL Array1D<Real64> XM4T;
    EP_GLOBAL Array1D<Real64> DSXMAT; // Down Stepped MAT history storage
    EP_GLOBAL Array1D<Real64> DSXM2T; // Down Stepped MAT history storage
    EP_GLOBAL Array1D<Real64> DSXM3T; // Down Stepped MAT history storage
    EP_GLOBAL Array1D<Real64> DSXM4T; // Down Stepped MAT history storage
    EP_GLOBAL Array1D<Real64> XMPT;   // Zone air temperature at previous time step

    EP_GLOBAL Array1D<Real64> ZTAVComf; // Zone Air Temperature Averaged over the Zone Time step used
    // in thermal comfort models (currently Fang model only)
    EP_GLOBAL Array1D<Real64> ZoneAirHumRatAvgComf; // AIR Humidity Ratio averaged over the zone time
    // step used in thermal comfort models (currently Fang model only)

    // Zone Air moisture conditions variables
    EP_GLOBAL Array1D<Real64> ZoneAirHumRatAvg;  // AIR Humidity Ratio averaged over the zone time step
    EP_GLOBAL Array1D<Real64> ZoneAirHumRat;     // AIR Humidity Ratio
    EP_GLOBAL Array1D<Real64> WZoneTimeMinus1;   // Humidity ratio history terms for 3rd order derivative
    EP_GLOBAL Array1D<Real64> WZoneTimeMinus2;   // Time Minus 2 Zone Time Steps Term
    EP_GLOBAL Array1D<Real64> WZoneTimeMinus3;   // Time Minus 3 Zone Time Steps Term
    EP_GLOBAL Array1D<Real64> WZoneTimeMinus4;   // Time Minus 4 Zone Time Steps Term
    EP_GLOBAL Array1D<Real64> DSWZoneTimeMinus1; // DownStepped Humidity ratio history terms for 3rd order derivative
    EP_GLOBAL Array1D<Real64> DSWZoneTimeMinus2; // DownStepped Time Minus 2 Zone Time Steps Term
    EP_GLOBAL Array1D<Real64> DSWZoneTimeMinus3; // DownStepped Time Minus 3 Zone Time Steps Term
    EP_GLOBAL Array1D<Real64> DSWZoneTimeMinus4; // DownStepped Time Minus 4 Zone Time Steps Term
    EP_GLOBAL Array1D<Real64> WZoneTimeMinusP;   // Humidity ratio history terms at previous time step

    EP_GLOBAL Array1D<Real64> ZoneAirHumRatTemp;   // Temp zone air humidity ratio at time plus 1
    EP_GLOBAL Array1D<Real64> WZoneTimeMinus1Temp; // Zone air humidity ratio at previous timestep
    EP_GLOBAL Array1D<Real64> WZoneTimeMinus2Temp; // Zone air humidity ratio at timestep T-2
    EP_GLOBAL Array1D<Real64> WZoneTimeMinus3Temp; // Zone air humidity ratio at timestep T-3
    EP_GLOBAL Array1D<Real64> ZoneAirHumRatOld;    // Last Time Steps Zone AIR Humidity Ratio

    EP_GLOBAL Array1D<Real64> MCPI;                       // INFILTRATION MASS FLOW * AIR SPECIFIC HEAT
    EP_GLOBAL Array1D<Real64> MCPTI;                      // INFILTRATION MASS FLOW * AIR CP * AIR TEMPERATURE
    EP_GLOBAL Array1D<Real64> MCPV;                       // VENTILATION MASS FLOW * AIR SPECIFIC HEAT
    EP_GLOBAL Array1D<Real64> MCPTV;                      // VENTILATION MASS FLOW * AIR CP * AIR TEMPERATURE
    EP_GLOBAL Array1D<Real64> MCPM;                       // Mixing MASS FLOW * AIR SPECIFIC HEAT
    EP_GLOBAL Array1D<Real64> MCPTM;                      // Mixing MASS FLOW * AIR CP * AIR TEMPERATURE
    EP_GLOBAL Array1D<Real64> MCPE;                       // EARTHTUBE MASS FLOW * AIR SPECIFIC HEAT
    EP_GLOBAL Array1D<Real64> EAMFL;                      // OUTDOOR AIR MASS FLOW for EarthTube
    EP_GLOBAL Array1D<Real64> EAMFLxHumRat;               // OUTDOOR AIR MASS FLOW * Humidity Ratio for EarthTube (water vapor mass flow)
    EP_GLOBAL Array1D<Real64> MCPTE;                      // EARTHTUBE MASS FLOW * AIR CP * AIR TEMPERATURE
    EP_GLOBAL Array1D<Real64> MCPC;                       // COOLTOWER MASS FLOW * AIR SPECIFIC HEAT
    EP_GLOBAL Array1D<Real64> CTMFL;                      // OUTDOOR AIR MASS FLOW for cooltower
    EP_GLOBAL Array1D<Real64> MCPTC;                      // COOLTOWER MASS FLOW * AIR CP * AIR TEMPERATURE
    EP_GLOBAL Array1D<Real64> ThermChimAMFL;              // OUTDOOR AIR MASS FLOW for THERMALCHIMNEY
    EP_GLOBAL Array1D<Real64> MCPTThermChim;              // THERMALCHIMNEY MASS FLOW * AIR SPECIFIC HEAT
    EP_GLOBAL Array1D<Real64> MCPThermChim;               // THERMALCHIMNEY MASS FLOW * AIR CP * AIR TEMPERATURE
    EP_GLOBAL Array1D<Real64> ZoneLatentGain;             // Latent Energy from each Zone (People, equipment)
    EP_GLOBAL Array1D<Real64> ZoneLatentGainExceptPeople; // Added for hybrid model -- Latent Energy from each Zone (equipment)
    EP_GLOBAL Array1D<Real64> OAMFL;                      // OUTDOOR AIR MASS FLOW (M**3/SEC) for infiltration
    EP_GLOBAL Array1D<Real64> VAMFL;                      // OUTDOOR AIR MASS FLOW (M**3/SEC) for ventilation
    EP_GLOBAL Array1D<Real64> NonAirSystemResponse;       // Convective heat addition rate from non forced air
    // equipment such as baseboards plus heat from lights to
    EP_GLOBAL Array1D<Real64> SysDepZoneLoads; // Convective heat addition or subtraction rate from sources that
    // depend on what is happening with the HVAC system. Such as:
    // heat gain from lights to return air when return flow = 0; heat gain
    // from air flow windows to return air when return air flow = 0;
    // and heat removed by return air from refrigeration cases when
    // return air flow = 0.
    EP_GLOBAL Array1D<Real64> SysDepZoneLoadsLagged; // SysDepZoneLoads saved to be added to zone heat balance next
    // HVAC time step
    EP_GLOBAL Array1D<Real64> MDotCPOA; // Airbalance MASS FLOW * AIR SPECIFIC HEAT used at Air Balance Method = Quadrature in the ZoneAirBalance:OutdoorAir
    EP_GLOBAL Array1D<Real64> MDotOA;   // Airbalance MASS FLOW rate used at Air Balance Method = Quadrature in the ZoneAirBalance:OutdoorAir

    EP_GLOBAL Array1D<Real64> MixingMassFlowZone;    // Mixing MASS FLOW
    EP_GLOBAL Array1D<Real64> MixingMassFlowXHumRat; // Mixing MASS FLOW * Humidity Ratio

    EP_GLOBAL Array1D_bool ZoneMassBalanceFlag;  // zone mass flow balance flag
    EP_GLOBAL Array1D_bool ZoneInfiltrationFlag; // Zone Infiltration flag
    EP_GLOBAL Array1D_int ZoneReOrder;           // zone number reordered for zone mass balance

    // REAL Variables for the Heat Balance Simulation

    EP_GLOBAL Array1D<Real64> QRadSysSource;     // Current source/sink for a particular surface (radiant sys)
    EP_GLOBAL Array1D<Real64> TCondFDSourceNode; // Temperature of source/sink location in surface from CondFD algo
    EP_GLOBAL Array1D<Real64> QPVSysSource;      // Current source/sink for a surface (integrated PV sys)

    EP_GLOBAL Array1D<Real64> CTFTsrcConstPart; // Constant Outside Portion of the CTF calculation of
    // temperature at source
    EP_GLOBAL Array1D<Real64> CTFTuserConstPart; // Constant Outside Portion of the CTF calculation of
    // temperature at user specified location
    EP_GLOBAL Array1D<Real64> QHTRadSysSurf; // Current radiant heat flux at a surface due to the presence
    // of high temperature radiant heaters
    EP_GLOBAL Array1D<Real64> QHWBaseboardSurf; // Current radiant heat flux at a surface due to the presence
    // of hot water baseboard heaters
    EP_GLOBAL Array1D<Real64> QSteamBaseboardSurf; // Current radiant heat flux at a surface due to the presence
    // of steam baseboard heaters
    EP_GLOBAL Array1D<Real64> QElecBaseboardSurf; // Current radiant heat flux at a surface due to the presence
    // of electric baseboard heaters
    EP_GLOBAL Array1D<Real64> QCoolingPanelSurf; // Current radiant heat flux at a surface due to the presence
    // of simple cooling panels
    EP_GLOBAL Array1D<Real64> QRadSurfAFNDuct;     // Current radiant heat flux at a surface due to radiation from AFN ducts
    EP_GLOBAL Array1D<Real64> QPoolSurfNumerator;  // Current pool heat flux impact at the surface (numerator of surface heat balance)
    EP_GLOBAL Array1D<Real64> PoolHeatTransCoefs;  // Current pool heat transfer coefficients (denominator of surface heat balance)
    EP_GLOBAL Array1D<Real64> RadSysTiHBConstCoef; // Inside heat balance coefficient that is constant
    EP_GLOBAL Array1D<Real64> RadSysTiHBToutCoef;  // Inside heat balance coefficient that modifies Toutside
    EP_GLOBAL Array1D<Real64> RadSysTiHBQsrcCoef;  // Inside heat balance coefficient that modifies source/sink
    EP_GLOBAL Array1D<Real64> RadSysToHBConstCoef; // Outside heat balance coefficient that is constant
    EP_GLOBAL Array1D<Real64> RadSysToHBTinCoef;   // Outside heat balance coefficient that modifies Toutside
    EP_GLOBAL Array1D<Real64> RadSysToHBQsrcCoef;  // Outside heat balance coefficient that modifies source/sink

    // Moisture variables to carry info from HB to the Zone Temp Predictor-Corrector for Fan System
    EP_GLOBAL Array1D<Real64> SumHmAW;   // SUM OF ZONE AREA*Moist CONVECTION COEFF*INSIDE Humidity Ratio
    EP_GLOBAL Array1D<Real64> SumHmARa;  // SUM OF ZONE AREA*Moist CONVECTION COEFF*Rho Air
    EP_GLOBAL Array1D<Real64> SumHmARaW; // SUM OF ZONE AREA*Moist CONVECTION COEFF*Rho Air* Inside Humidity Ration
    EP_GLOBAL Array1D<Real64> SumHmARaZ;

    EP_GLOBAL Array1D<Real64> TempZoneThermostatSetPoint;
    EP_GLOBAL Array1D<Real64> AdapComfortCoolingSetPoint;
    EP_GLOBAL Array1D<Real64> ZoneThermostatSetPointHi;
    EP_GLOBAL Array1D<Real64> ZoneThermostatSetPointLo;
    EP_GLOBAL Array1D<Real64> ZoneThermostatSetPointHiAver;
    EP_GLOBAL Array1D<Real64> ZoneThermostatSetPointLoAver;

    EP_GLOBAL Array1D<Real64> LoadCorrectionFactor; // PH 3/3/04

    EP_GLOBAL Array1D<Real64> AIRRAT; // "air power capacity"  PH 3/5/04
    EP_GLOBAL Array1D<Real64> ZTM1;   // zone air temperature at previous timestep
    EP_GLOBAL Array1D<Real64> ZTM2;   // zone air temperature at timestep T-2
    EP_GLOBAL Array1D<Real64> ZTM3;   // zone air temperature at previous T-3
    // Hybrid Modeling
    EP_GLOBAL Array1D<Real64> PreviousMeasuredZT1;     // Hybrid model internal mass multiplier at previous timestep
    EP_GLOBAL Array1D<Real64> PreviousMeasuredZT2;     // Hybrid model internal mass multiplier at previous timestep
    EP_GLOBAL Array1D<Real64> PreviousMeasuredZT3;     // Hybrid model internal mass multiplier at previous timestep
    EP_GLOBAL Array1D<Real64> PreviousMeasuredHumRat1; // Hybrid model zone humidity ratio at previous timestep
    EP_GLOBAL Array1D<Real64> PreviousMeasuredHumRat2; // Hybrid model zone humidity ratio at previous timestep
    EP_GLOBAL Array1D<Real64> PreviousMeasuredHumRat3; // Hybrid model zone humidity ratio at previous timestep
    // Exact and Euler solutions
    EP_GLOBAL Array1D<Real64> ZoneTMX; // TEMPORARY ZONE TEMPERATURE TO TEST CONVERGENCE in Exact and Euler method
    EP_GLOBAL Array1D<Real64> ZoneTM2; // TEMPORARY ZONE TEMPERATURE at timestep t-2 in Exact and Euler method
    EP_GLOBAL Array1D<Real64> ZoneT1;  // Zone temperature at the previous time step used in Exact and Euler method
    EP_GLOBAL Array1D<Real64> ZoneWMX; // TEMPORARY ZONE TEMPERATURE TO TEST CONVERGENCE in Exact and Euler method
    EP_GLOBAL Array1D<Real64> ZoneWM2; // TEMPORARY ZONE TEMPERATURE at timestep t-2 in Exact and Euler method
    EP_GLOBAL Array1D<Real64> ZoneW1;  // Zone temperature at the previous time step used in Exact and Euler method

    EP_GLOBAL Array1D_int TempControlType;
    EP_GLOBAL Array1D_int ComfortControlType;

    // Object Data
    EP_GLOBAL Array1D<ZoneComfortControlsFangerData> ZoneComfortControlsFanger;

    void clear_state()
    {
        SumConvHTRadSys.deallocate();
        SumLatentHTRadSys.deallocate();
        SumConvPool.deallocate();
        SumLatentPool.deallocate();
        QHTRadSysToPerson.deallocate();
        QHWBaseboardToPerson.deallocate();
        QSteamBaseboardToPerson.deallocate();
        QElecBaseboardToPerson.deallocate();
        ZTAV.deallocate();
        MAT.deallocate();
        TempTstatAir.deallocate();
        ZT.deallocate();
        XMAT.deallocate();
        XM2T.deallocate();
        XM3T.deallocate();
        XM4T.deallocate();
        DSXMAT.deallocate();
        DSXM2T.deallocate();
        DSXM3T.deallocate();
        DSXM4T.deallocate();
        XMPT.deallocate();
        ZTAVComf.deallocate();
        ZoneAirHumRatAvgComf.deallocate();
        ZoneAirHumRatAvg.deallocate();
        ZoneAirHumRat.deallocate();
        WZoneTimeMinus1.deallocate();
        WZoneTimeMinus2.deallocate();
        WZoneTimeMinus3.deallocate();
        WZoneTimeMinus4.deallocate();
        DSWZoneTimeMinus1.deallocate();
        DSWZoneTimeMinus2.deallocate();
        DSWZoneTimeMinus3.deallocate();
        DSWZoneTimeMinus4.deallocate();
        WZoneTimeMinusP.deallocate();
        ZoneAirHumRatTemp.deallocate();
        WZoneTimeMinus1Temp.deallocate();
        WZoneTimeMinus2Temp.deallocate();
        WZoneTimeMinus3Temp.deallocate();
        ZoneAirHumRatOld.deallocate();
        MCPI.deallocate();
        MCPTI.deallocate();
        MCPV.deallocate();
        MCPTV.deallocate();
        MCPM.deallocate();
        MCPTM.deallocate();
        MCPE.deallocate();
        EAMFL.deallocate();
        EAMFLxHumRat.deallocate();
        MCPTE.deallocate();
        MCPC.deallocate();
        CTMFL.deallocate();
        MCPTC.deallocate();
        ThermChimAMFL.deallocate();
        MCPTThermChim.deallocate();
        MCPThermChim.deallocate();
        ZoneLatentGain.deallocate();
        ZoneLatentGainExceptPeople.deallocate();
        OAMFL.deallocate();
        VAMFL.deallocate();
        NonAirSystemResponse.deallocate();
        SysDepZoneLoads.deallocate();
        SysDepZoneLoadsLagged.deallocate();
        MDotCPOA.deallocate();
        MDotOA.deallocate();
        MixingMassFlowZone.deallocate();
        MixingMassFlowXHumRat.deallocate();
        ZoneMassBalanceFlag.deallocate();
        ZoneInfiltrationFlag.deallocate();
        ZoneReOrder.deallocate();
        QRadSysSource.deallocate();
        TCondFDSourceNode.deallocate();
        QPVSysSource.deallocate();
        CTFTsrcConstPart.deallocate();
        CTFTuserConstPart.deallocate();
        QHTRadSysSurf.deallocate();
        QHWBaseboardSurf.deallocate();
        QSteamBaseboardSurf.deallocate();
        QElecBaseboardSurf.deallocate();
        QPoolSurfNumerator.deallocate();
        QRadSurfAFNDuct.deallocate();
        PoolHeatTransCoefs.deallocate();
        RadSysTiHBConstCoef.deallocate();
        RadSysTiHBToutCoef.deallocate();
        RadSysTiHBQsrcCoef.deallocate();
        RadSysToHBConstCoef.deallocate();
        RadSysToHBTinCoef.deallocate();
        RadSysToHBQsrcCoef.deallocate();
        SumHmAW.deallocate();
        SumHmARa.deallocate();
        SumHmARaW.deallocate();
        TempZoneThermostatSetPoint.deallocate();
        AdapComfortCoolingSetPoint.deallocate();
        ZoneThermostatSetPointHi.deallocate();
        ZoneThermostatSetPointLo.deallocate();
        ZoneThermostatSetPointHiAver.deallocate();
        ZoneThermostatSetPointLoAver.deallocate();
        LoadCorrectionFactor.deallocate();
        AIRRAT.deallocate();
        ZTM1.deallocate();
        ZTM2.deallocate();
        ZTM3.deallocate();
        PreviousMeasuredZT1.deallocate();
        PreviousMeasuredZT2.deallocate();
        PreviousMeasuredZT3.deallocate();
        PreviousMeasuredHumRat1.deallocate();
        PreviousMeasuredHumRat2.deallocate();
        PreviousMeasuredHumRat3.deallocate();
        ZoneTMX.deallocate();
        ZoneTM2.deallocate();
        ZoneT1.deallocate();
        ZoneWMX.deallocate();
        ZoneWM2.deallocate();
        ZoneW1.deallocate();
        TempControlType.deallocate();
        ComfortControlType.deallocate();
        ZoneComfortControlsFanger.deallocate();
    }

} // namespace DataHeatBalFanSys

} // namespace EnergyPlus
