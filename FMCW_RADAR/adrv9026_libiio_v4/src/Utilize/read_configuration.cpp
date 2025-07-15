#include "../../include/Utilize/read_configuration.h"


bool read_config_file(std::string filename, 
  std::unordered_map<std::string, std::vector<float>> & confData) {
  std::ifstream configFile(filename.c_str());
  if (!configFile.is_open()) {
    std::cout << "Parse Config: cannot open config file, using default file" << std::endl;
    return false;
  }
  
  std::string line;
  while (std::getline(configFile, line)) {
    std::istringstream iss(line);
    std::string key;
    iss >> key;

    float value;
    std::vector<float> values;
    while (iss >> value) {
      values.push_back(value);
    }
    confData[key] = values;
  }
  
  configFile.close();
  return true;
}
  
void parse_config_file(std::unordered_map<std::string, std::vector<float>> & confData, 
  int & fmcw_enable,
  struct MIMO_LFM_conf & config,
  struct x1d_cacfar_param & xparams,
  struct PulseRadar & pconfig,
  struct stream_cfg & trxcfg,
  struct process_param & params,
  bool ec[4]) {
  
  std::cout << "loading config file\n";
  
  for (const auto& [key, values] : confData) {
    std::cout << key << ": ";
    
    for (float v : values) {
      std::cout << v << " ";
    }
    std::cout << std::endl;
    
    int index = 0;
    if (key == std::string("mimo")) {
      config.fs            = 245.76e6; // 122e6; // fixed for adrv9025
      for (float v : values) {
        if (index == 0) config.B = v * 1e6;
        if (index == 1) config.PRI = v * 1e-6;
        if (index == 2) config.tp = v * 1e-6;
        if (config.tp > config.PRI) config.tp = config.PRI;
        if (index == 3) config.PulseNum = v;
        if (index == 4) config._use_half_band_ = v;
        if (index == 5) config.move = v;
        index++;
      }
     	config.PulseLength   = config.fs * config.PRI;
      config.SignalLength  = config.fs * config.tp;
     	config.K             = config.B / config.tp;
      config.FMCW_wave     = Eigen::ArrayXXcf::Zero(1, config.PulseLength);
    }
    else if (key == std::string("LO")) {
      for (float v : values) {
        trxcfg.lo_hz = long(v * 1e9);        
      }
    }
    else if (key == std::string("CFAR")) {
      for (float v : values) {
        if (index == 0) xparams.R = v;
        if (index == 1) xparams.P = v;
        if (index == 2) xparams.Pfa = v;
        index++;
      }
    }
    else if (key == std::string("EC")) {
      for (float v : values) {
        ec[index] = v;
        index++;
      }
    }
    else if (key == std::string("process")) {
      for (float v: values) {
        if (index == 0) params.ori_fs = v * 1e6;
        if (index == 1) params.ex_fs = v * 1e6;
        if (index == 2) params.cancelnum = v;
        if (index == 3) params.mtd_cancel = v; // mtd cancel enable = true or false
        index++;
      }
    }else if (key == std::string("type")) {
      for (float v:values) {
        if (index == 0) fmcw_enable = v;
        index++;
      }
    }else if (key == std::string("pulse")) {
      pconfig.fs = 245.76e6;
      pconfig.c  = 3e8;
      for (float v:values) {
        if (index == 0) pconfig.tp  = v * 1e-6;
        if (index == 1) pconfig.PRI = v * 1e-6;
        if (index == 2) pconfig.PN  = v;
        if (index == 3) pconfig.B   = v * 1e6;
        if (index == 4) pconfig.can_num = v;         // cancellation number
        if (index == 5) pconfig._use_half_band_ = v; // half band enable
        index++;
      }
      pconfig.K = pconfig.B / pconfig.tp;
      pconfig.siglen = (pconfig.PRI + pconfig.tp) * pconfig.fs;
      pconfig.prilen = pconfig.PRI * pconfig.fs;
      pconfig.tplen  = pconfig.tp * pconfig.fs;
    }
    
  }

}