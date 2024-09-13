
struct RayPayload
{
    vec4  ColorAndDistance;    // rgb + t
    vec4  EmitColor;
    vec4  ScatterDirection;    // xyz + w (is scatter needed)
    uint  RandomSeed;
    float Pdf;
    bool  SkipPdf;
    float ScatterPdf;
};
