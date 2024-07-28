#pragma once

namespace UI
{

struct UserSettings final
{
    // Devices
    std::vector<uint32_t> VisibleDevices{};

    // Application
    bool Benchmark;

    // Benchmark
    bool     BenchmarkNextScenes{};
    uint32_t BenchmarkMaxTime{};

    // Scene
    int SceneIndex;

    // Renderer
    bool     IsRayTraced;
    bool     AccumulateRays;
    uint32_t NumberOfSamples;
    uint32_t NumberOfBounces;
    uint32_t MaxNumberOfSamples;

    // Camera
    float FieldOfView;
    float Aperture;
    float FocusDistance;

    // Profiler
    bool  ShowHeatmap;
    float HeatmapScale;

    // UI
    bool ShowSettings;
    bool ShowViewport;
    bool ShowStats;

    static constexpr float FieldOfViewMinValue = 10.0f;
    static constexpr float FieldOfViewMaxValue = 90.0f;

    bool RequiresAccumulationReset(const UserSettings& prev) const
    {
        return IsRayTraced != prev.IsRayTraced
               || AccumulateRays != prev.AccumulateRays
               || NumberOfBounces != prev.NumberOfBounces
               || FieldOfView != prev.FieldOfView
               || Aperture != prev.Aperture
               || FocusDistance != prev.FocusDistance;
    }
};

}    // namespace UI
