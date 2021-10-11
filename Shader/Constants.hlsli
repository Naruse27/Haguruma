//cbuffer OBJECT_CONSTANTS : register(b0)
//{
//    row_major float4x4 world;
//};

//static const int MAX_BONES = 256;
//cbuffer BONE_CONSTANTS : register(b1)
//{
//    row_major float4x4 boneTransforms[MAX_BONES];
//};

//cbuffer MATERIAL_CONSTANTS : register(b2)
//{
//    float4 ambient;
//    float4 diffuse; // The w component is 'opacity'.
//    float4 specular; // The w component is 'shininess'.
//    float4 reflection;
//    float4 emissive;
//    float2 texcoordOffset;
//    float2 texcoordScale;
//};

//cbuffer SCENE_CONSTANTS : register(b3)
//{
//    row_major float4x4 view_projection;
//    row_major float4x4 inverse_view_projection;
//    float4 directional_light_direction[1];
//    float4 directional_light_color[1]; // The w component is 'intensity'.
//    float4 omni_light_position[1];
//    float4 omni_light_color[1]; // The w component is 'intensity'.
//    float4 rimlight_color;
//    float4 camera_position;
//    float4 avatar_position;
//    float4 avatar_direction;
//    row_major float4x4 light_view_projection;
//    float time;
//    float wind_frequency;
//    float wind_strength;
//    float rimlight_factor;
//};

//cbuffer GRASS_CONSTANTS : register(b4)
//{
//    float grass_height_factor;
//    float grass_width_factor;
//    float grass_curvature;
//    float grass_withered_factor;
//    float grass_height_variance;
//    float perlin_noise_distribution_factor;
//    float tesselation_max_subdivision;
//    float tesselation_max_distance;
//    float4 grass_specular_color;
//};

//cbuffer SHADOW_MAP_CONSTANTS : register(b5)
//{
//    float shadow_depth_bias;
//    float shadow_color;
//    float shadow_filter_radius;
//    uint shadow_sample_count;
//};

//cbuffer BLOOM_CONSTANTS : register(b6)
//{
//    float bloom_extraction_threshold;
//    float blur_convolution_intensity;

//    float2 bloom_options;
//};

//cbuffer MIST_CONSTANTS : register(b7)
//{
//    float4 mist_color;
//    float2 mist_density; // x:extinction, y:inscattering
//    float2 mist_height_falloff; // x:extinction, y:inscattering
//    float2 height_mist_offset; // x:extinction, y:inscattering
	
//    float mist_cutoff_distance;
	
//    float mist_flow_speed;
//    float mist_flow_noise_scale_factor;
//    float mist_flow_density_lower_limit;
	
//    float distance_to_sun;
//    float sun_highlight_exponential_factor; // Affects the area of influence of the sun's highlights.
//    float sun_highlight_intensity;
	
//    float3 mist_options;
//};

//cbuffer POST_EFFECT_CONSTANTS : register(b8)
//{
//    float brightness;
//    float contrast;
//    float hue;
//    float saturation;

//    float bokeh_aperture;
//    float bokeh_focus;

//    float exposure;
//    float post_effect_options;

//    float4 pantone;
//};