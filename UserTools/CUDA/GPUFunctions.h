#ifndef GPU_FUNCTIONS_H
#define GPU_FUNCTIONS_H

#include <vector>

namespace GPU_daq {

int test_vertices_initialize();
int test_vertices_execute();
int test_vertices_finalize();
int nhits_initialize();
 int nhits_initialize_ToolDAQ(std::string ParameterFile,std::vector<int> tube_no,std::vector<float> tube_x,std::vector<float> tube_y,std::vector<float> tube_z, int fTriggerSearchWindow, int fTriggerSearchWindowStep, int fTriggerThreshold, int fTriggerSaveWindowPre, int fTriggerSaveWindowPost);
int nhits_execute();
int nhits_execute(std::vector<int> PMTid, std::vector<int> time, std::vector<int> * trigger_ns, std::vector<int> * trigger_ts);
int nhits_finalize();

//int CUDAFunction(std::vector<int> PMTid, std::vector<int> time);

};

#endif
