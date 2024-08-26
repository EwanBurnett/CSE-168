#include "Utils/Logger.h"
#include "Utils/Timer.h"
#include "Maths.h"

constexpr uint32_t NUM_MATRICES = 1 << 20; 

int main() {
    EDX::Log::Status("Computing %d matrix multiplications.\n", NUM_MATRICES);

    //Resource Allocation
    EDX::Maths::Matrix4x4<float>* mats_a = new EDX::Maths::Matrix4x4<float>[NUM_MATRICES]; 
    EDX::Maths::Matrix4x4<float>* mats_b = new EDX::Maths::Matrix4x4<float>[NUM_MATRICES]; 
    EDX::Maths::Matrix4x4<float>* result = new EDX::Maths::Matrix4x4<float>[NUM_MATRICES]; 

    
    //Initialisation
    {
        EDX::Timer timer; 
        timer.Start(); 
        
        for (uint32_t i = 0; i < NUM_MATRICES; i++) {
            double degrees = (static_cast<double>(i) / static_cast<double>(NUM_MATRICES)) * 360.0; 
            mats_a[i] = EDX::Maths::Matrix4x4<float>::XRotationFromDegrees(degrees);
            mats_b[i] = EDX::Maths::Matrix4x4<float>::ZRotationFromDegrees(degrees);
        }
        timer.Tick(); 

        const double initialization_time_s = timer.DeltaTime(); 
        EDX::Log::Success("Initialization Complete in %.8fs.\n", initialization_time_s); 
    }

    //Computation
    {
        EDX::Timer timer; 
        timer.Start(); 

        for (uint32_t i = 0; i < NUM_MATRICES; i++) {
            result[i] = mats_a[i] * mats_b[i]; 
        }
        timer.Tick(); 

        const double computation_time_s = timer.DeltaTime(); 
        EDX::Log::Success("Computation Complete in %.8fs.\n", computation_time_s); 
    }

    //Clean-up
    delete[] result; 
    delete[] mats_b; 
    delete[] mats_a; 
}