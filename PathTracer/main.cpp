#include "Utils/Logger.h"
#include "Utils/Timer.h"
#include "Maths.h"
#include "Utils/ProgressBar.h"

constexpr uint32_t NUM_MATRICES = 1 << 18; 

int main() {
    EDX::Log::Status("Computing %d matrix multiplications.\n", NUM_MATRICES);

    //Resource Allocation
    EDX::Maths::Matrix4x4<float>* mats_a = new EDX::Maths::Matrix4x4<float>[NUM_MATRICES]; 
    EDX::Maths::Matrix4x4<float>* mats_b = new EDX::Maths::Matrix4x4<float>[NUM_MATRICES]; 
    EDX::Maths::Matrix4x4<float>* result = new EDX::Maths::Matrix4x4<float>[NUM_MATRICES]; 

    
    //Initialisation
    {
        EDX::Log::Status("Initializing...\n");
        EDX::ProgressBar pb;
        
        for (uint32_t i = 0; i < NUM_MATRICES; i++) {
            double degrees = (static_cast<double>(i) / static_cast<double>(NUM_MATRICES)) * 360.0; 
            mats_a[i] = EDX::Maths::Matrix4x4<float>::XRotationFromDegrees(degrees);
            mats_b[i] = EDX::Maths::Matrix4x4<float>::ZRotationFromDegrees(degrees);

            float p = (float)i / (float)NUM_MATRICES; 
            pb.Update(p); 
        }

        const double initialization_time_s = pb.GetProgressTimer().Duration();
        EDX::Log::Success("\nInitialization Complete in %.8fs.\n", initialization_time_s); 
    }

    //Computation
    //For now, we'll use this to simulate doing some work, i.e. Rendering an image.  
    {
        EDX::Log::Status("Computing Data...\n");
        EDX::ProgressBar pb;

        for (uint32_t i = 0; i < NUM_MATRICES; i++) {

            result[i] = mats_a[i] * mats_b[i]; 
            float p = (float)i / (float)NUM_MATRICES; 
            pb.Update(p); 
        }

        const double computation_time_s = pb.GetProgressTimer().Duration();
        EDX::Log::Success("\nComputation Complete in %.8fs.\n", computation_time_s); 
    }

    //Clean-up
    delete[] result; 
    delete[] mats_b; 
    delete[] mats_a; 
}