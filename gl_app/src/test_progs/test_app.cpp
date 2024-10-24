#include "pch.h"
#include "camera.h"
#include "window.h"
#include "test_app.h"

#include "misc/coords/coord_sys.h"
#include "misc/anim_model/anim_model.h"
#include "misc/comp_geom/comp_geom.h"
#include "misc/dir_shadows_visualised/shadow_mapping.h"
#include "misc/csm_shadows_visualised/csm.h"

#include "1.getting_started/4.1.textures/texture_test.h"
#include "1.getting_started/4.2.textures_combined/textures_combined.h"
#include "2.lighting/basic_lighting_colours.h"
#include "2.lighting/basic_lighting_diffuse.h"
#include "2.lighting/basic_lighting_specular.h"
#include "2.lighting/materials.h"
#include "2.lighting/light_map_diffuse.h"
#include "2.lighting/light_map_specular.h"
#include "2.lighting/light_map_emission.h"
#include "2.lighting/light_caster_directional.h"
#include "2.lighting/light_caster_point.h"
#include "2.lighting/light_caster_spot.h"
#include "2.lighting/light_caster_spot_soft.h"
#include "2.lighting/multiple_lights.h"
#include "3.model_loading/model_loading.h"
#include "4.advanced_opengl/1.1.depth_testing/depth_testing.h"
#include "4.advanced_opengl/1.2.depth_testing_view/depth_testing_view.h"
#include "4.advanced_opengl/2.stencil_testing/stencil_testing.h"
#include "4.advanced_opengl/3.1.blending_discard/blending_discard.h"
#include "4.advanced_opengl/3.2.blending_sort/blending_sort.h"
#include "4.advanced_opengl/4.face_culling/face_culling.h"
#include "4.advanced_opengl/5.1.frame_buffers/frame_buffers.h"
#include "4.advanced_opengl/6.1.cubemaps_skybox/cubemaps_skybox.h"
#include "4.advanced_opengl/6.2.cubemaps_environment_mapping/cubmaps_environment_mapping.h"
#include "4.advanced_opengl/8.advanced_glsl_ubo/advanced_glsl_ubo.h"
#include "4.advanced_opengl/9.1.geometry_shader_houses/geometry_shader_houses.h"
#include "4.advanced_opengl/9.2.geometry_shader_exploding/geometry_shader_exploding.h"
#include "4.advanced_opengl/9.3.geometry_shader_normals/normal_visualisation.h"
#include "4.advanced_opengl/10.1.instancing_quads/instancing_quads.h"
#include "4.advanced_opengl/10.2.asteroids/asteroids.h"
#include "4.advanced_opengl/10.3.asteroids_instanced/asteroids_instanced.h"
#include "4.advanced_opengl/11.1.anti_aliasing_msaa/anti_aliasing_msaa.h"
#include "4.advanced_opengl/11.2.anti_aliasing_offscreen/anti_aliasing_offscreen.h"
#include "5.advanced_lighting/1.advanced_lighting/advanced_lighting.h"
#include "5.advanced_lighting/2.gamma_correction/gamma_correction.h"
#include "5.advanced_lighting/3.1.1.shadow_mapping_depth/shadow_mapping_depth.h"
#include "5.advanced_lighting/3.1.2.shadow_mapping_base/shadow_mapping_base.h"
#include "5.advanced_lighting/3.1.3.shadow_mapping/shadow_mapping.h"
#include "5.advanced_lighting/3.2.1.point_shadows/point_shadows.h"
#include "5.advanced_lighting/3.2.2.point_shadows_soft/point_shadows_soft.h"
#include "5.advanced_lighting/4.normal_mapping/normal_map.h"
#include "5.advanced_lighting/5.1.parallax_mapping/parallax_mapping.h"
#include "5.advanced_lighting/5.2.parallax_mapping/parallax_mapping_multi_sample.h"
#include "5.advanced_lighting/5.3.parallax_mapping/parallax_occlusion_mapping.h"
#include "5.advanced_lighting/6.hdr/hdr.h"
#include "5.advanced_lighting/7.bloom/bloom.h"
#include "5.advanced_lighting/8.1.deferred_shading/deferred_shading.h"
#include "5.advanced_lighting/9.1.ssao/ssao.h"
#include "6.pbr/1.pbr_basic/pbr_basic.h"
#include "6.pbr/2.pbr_textured/pbr_textured.h"
#include "6.pbr/3.1.ibl_diffuse_irradiance/ibl_diffuse_irradiance_1.h"
#include "6.pbr/3.2.ibl_diffuse_irradiance/ibl_diffuse_irradiance_2.h"
#include "7.in_practice/2.text_rendering/text_rendering.h"
#include "8.guest/2020/skeletal_animation/skeletal_animation.h"
#include "8.guest/2021/csm/csm.h"
#include "8.guest/2021/scene/scene_graph/scene_graph.h"
#include "8.guest/2021/tesselation/height_maps/height_map_cpu.h"
#include "8.guest/2021/tesselation/tesselation/tesselation.h"

#include "opengl_bible/ch2/ch2.1_point.h"
#include "opengl_bible/ch2/ch2.2_triangle.h"
#include "opengl_bible/ch3/ch3.1_moving_triangle.h"
#include "opengl_bible/ch3/ch3.2_tesselated_triangle.h"
#include "opengl_bible/ch3/ch3.3_geom_triangle.h"
#include "opengl_bible/ch5/buffers/vertex_array_4_5.h"




TestAppMgr::TestAppMgr(Window& window, Camera& camera, v2::Camera& camera2) :
	m_window(window), m_camera(camera), m_camera2(camera2)
{
}

Layer* TestAppMgr::GetLayer(uint32_t id)
{
	switch (id)
	{
		case COORD_SYS_CAM1: return new CoordSys(&m_camera, nullptr);
		case COORD_SYS_CAM2: return new CoordSys(nullptr, &m_camera2);

		case LGL_TEXTURE_TEST: return new TextureTest(); break;
		case LGL_TEXTURES_COMBINED_TEST: return new TexturesCombinedTest(); break;
		case LGL_BASIC_LIGHTING_COLOURS: return new BasicLightingColours(m_camera); break;
		case LGL_BASIC_LIGHTING_DIFFUSE: return new BasicLightingDiffuse(m_camera); break;
		case LGL_BASIC_LIGHTING_SPECULAR: return new BasicLightingSpecular(m_camera); break;
		case LGL_MATERIALS_TEST: return new MaterialsTest(m_camera); break;
		case LGL_LIGHT_MAP_DIFFUSE: return new LightMapDiffuse(m_camera); break;
		case LGL_LIGHT_MAP_SPECULAR: return new LightMapSpecular(m_camera); break;
		case LGL_LIGHT_MAP_EMISSION: return new LightMapEmission(m_camera); break;
		case LGL_LIGHT_CASTER_DIRECTIONAL: return new LightCasterDirectional(m_camera); break;
		case LGL_LIGHT_CASTER_POINT: return new LightCasterPoint(m_camera); break;
		case LGL_LIGHT_CASTER_SPOT: return new LightCasterSpot(m_camera); break;
		case LGL_LIGHT_CASTER_SPOT_SOFT: return new LightCasterSpotSoft(m_camera); break;
		case LGL_MULTIPLE_LIGHTS: return new MultipleLights(m_camera); break;

		case LGL_MODEL_LOADING_TEST: return new ModelLoadingTest(m_camera); break;	

		case LGL_DEPTH_TESTING: return new DepthTesting(m_camera); break;
		case LGL_DEPTH_TESTING_VIEW: return new DepthTestingView(m_camera); break;
		case LGL_STENCIL_TESTING: return new StencilTesting(m_camera); break;
		case LGL_BLEND_DISCARD_TESTING: return new BlendingDiscardTest(m_camera); break;
		case LGL_BLEND_SORT_TESTING: return new BlendingSortTest(m_camera); break;
		case LGL_FACE_CULLING: return new FaceCulling(m_camera); break;
		case LGL_FRAME_BUFFER_TEST: return new FrameBufferTest(m_window, m_camera); break;
		case LGL_SKYBOX: return new CubeMapSkyBox(m_camera); break;
		case LGL_ENV_MAPPING: return new CubeMapEnvironmentMapping(m_camera); break;
		case LGL_ADVANCED_GLSL_UBO: return new AdvancedGLSL_UBO(m_camera); break;
		case LGL_GEOMETRY_SHADER_HOUSES: return new GeometryShaderHouses(); break;
		case LGL_GEOMETRY_SHADER_EXPLODING: return new GeometryShaderExploding(m_camera); break;
		case LGL_NORMAL_VISUALISATION: return new NormalVisualisation(m_camera); break;
		case LGL_INSTANCING_QUADS: return new InstancingQuads(m_camera); break;
		case LGL_ASTEROIDS: return new Asteroids(m_camera); break;
		case LGL_ASTEROIDS_INSTANCED: return new AsteroidsInstanced(m_camera); break;
		case LGL_ANTI_ALIASING_MSAA: return new AntiAliasingMSAA(m_camera); break;
		case LGL_ANTI_ALIASING_OFFSCREEN: return new AntiAliasingOffscreen(m_camera,m_window); break;

		case LGL_ADVANCED_LIGHTING: return new AdvancedLighting(m_camera, m_window); break;
		case LGL_GAMMA_CORRECTION: return new GammaCorrection(m_camera, m_window); break;
		case LGL_SHADOW_MAPPING_DEPTH: return new ShadowMappingDepth(m_window, m_camera); break;
		case LGL_SHADOW_MAPPING_BASE: return new ShadowMappingBase(m_window, m_camera); break;
		case LGL_SHADOW_MAPPING: return new ShadowMapping(m_window, m_camera); break;
		case LGL_POINT_SHADOW: return new PointShadows(m_window, m_camera); break;
		case LGL_POINT_SHADOW_SOFT: return new PointShadowsSoft(m_window, m_camera); break;
		case LGL_NORMAL_MAPPING: return new NormalMap(m_window, m_camera); break;
		case LGL_PARALLAX_MAPPING: return new ParallaxMapping(m_window, m_camera); break;
		case LGL_PARALLAX_MAPPING_MULTI_SAMPLE: return new ParallaxMappingMultiSample(m_window, m_camera); break;
		case LGL_PARALLAX_OCCLUSION_MAPPING: return new ParallaxOcclusionMapping(m_window, m_camera); break;
		case LGL_HDR: return new HDR(m_window, m_camera); break;
		case LGL_BLOOM: return new Bloom(m_window, m_camera); break;
		case LGL_DEFERRED_SHADING: return new DeferredShading(m_window, m_camera); break;
		case LGL_SCREEN_SPACE_AMBIENT_OCCLUSION: return new SSAO(m_window, m_camera); break;
	
		case LGL_TEXT_RENDERING: return new TextRendering(m_window, m_camera); break;

		case LGL_SKELETAL_ANIMATION: return new SkeletalAnimation(m_window, m_camera); break;
		case LGL_CSM: return new CSM(m_window, m_camera); break;
		case LGL_SCENE_GRAPH: return new SceneGraph(m_window, m_camera); break;
		case LGL_HEIGHT_MAP_CPU: return new LayerHeightMapCPU(m_camera); break;
		case LGL_HEIGHT_MAP_TESSELATION: return new Tesselation(m_window, m_camera);

		case SB7_POINT: return new sb7::PointTest(); break;
		case SB7_TRIANGLE: return new sb7::TriangleTest(); break;
		case SB7_MOVING_TRIANGLE: return new sb7::MovingTriangle(); break;
		case SB7_TESSELATED_TRIANGLE: return new sb7::TesselatedTriangle(); break;
		case SB7_GEOM_TRIANGLE: return new sb7::GeomTriangle(); break;
		case SB7_CH5_VERTEX_ARRAYS: return new sb7::LightMapSpecular(m_camera); break;

		case MISC_ANIMATED_MODEL_BASIC: return new me::BasicModelTest(m_window, m_camera); break;
		case MISC_COMP_GEOM: return new jmk::CompGeom(m_window, m_camera); break;
		
			//These use m_camera2
		case MISC_DIR_SHADOW_VISUALISED: return new me::ShadowMappingVisualised(m_window, m_camera2);
		case MISC_CSM_VISUALISED: return new me::CSMVisualised(m_window, m_camera2); break;
		case LGL_PBR_BASIC: return new PbrBasic(m_window, m_camera2); break;
		case LGL_PBR_TEXTURED: return new PbrTextured(m_window, m_camera2); break;
		case LGL_IBR_DIFFUSE_IRRADIANCE_1: return new IblDiffuseIrradiance1(m_window, m_camera2); break;
		case LGL_IBR_DIFFUSE_IRRADIANCE_2: return new IblDiffuseIrradiance2(m_window, m_camera2); break;
	}
	return new CoordSys(&m_camera, nullptr);
}

