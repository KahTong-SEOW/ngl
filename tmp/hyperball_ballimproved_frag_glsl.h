// Copyright (C) 2010-2011 by
// Laboratoire de Biochimie Theorique (CNRS),
// Laboratoire d'Informatique Fondamentale d'Orleans (Universite d'Orleans), (INRIA) and
// Departement des Sciences de la Simulation et de l'Information (CEA). 

// License: CeCILL-C license (http://www.cecill.info/)

// Contact: Marc Baaden
// E-mail: baaden@smplinux.de
// Webpage: http://hyperballs.sourceforge.net

#ifndef __BALLIMPROVED_FRAG_H__
#define __BALLIMPROVED_FRAG_H__
const char ballimproved_frag[] = ""
"\n"
"#extension GL_ARB_texture_rectangle : enable\n"
"\n"
"\n"
"varying vec4 i_near;\n"
"varying vec4 i_far;\n"
"varying vec4 sphereposition;\n"
"varying vec4 color;\n"
"varying float radius;\n"
"\n"
"\n"
"struct Ray{\n"
"   vec3 origin;\n"
"   vec3 direction;\n"
"};\n"
"\n"
"vec3 isect_surf(Ray r, mat4 matrix_coef){\n"
"   vec4 direction = vec4(r.direction, 0.0);\n"
"   vec4 origin = vec4(r.origin, 1.0);\n"
"   float a = dot(direction,(matrix_coef*direction));\n"
"   float b = dot(origin,(matrix_coef*direction));\n"
"   float c = dot(origin,(matrix_coef*origin));\n"
"   float delta =b*b-a*c;\n"
"   if (delta<0.0) discard;\n"
"   float t1 =(-b-sqrt(delta))/a;\n"
"\n"
"   // Second solution not necessary if you don't want\n"
"   // to see inside spheres and cylinders, save some fps\n"
"   //float t2 = (-b+sqrt(delta)) / a  ;\n"
"   //float t =(t1<t2) ? t1 : t2;\n"
"\n"
"   return r.origin+t1*r.direction;\n"
"}\n"
"\n"
"Ray primary_ray(vec4 near1, vec4 far1){\n"
"    vec3 near=near1.xyz/near1.w;\n"
"    vec3 far=far1.xyz/far1.w;\n"
"    return Ray(near,far-near);\n"
"}\n"
"\n"
"float update_z_buffer(vec3 M, mat4 ModelViewP){\n"
"    float  depth1;\n"
"    vec4 Ms=(ModelViewP*vec4(M,1.0));\n"
"    return depth1=(1.0+Ms.z/Ms.w)/2.0;\n"
"}\n"
"\n"
"vec4 lit(float NdotL, float NdotH, float m) {\n"
"  float ambient = 1.0;\n"
"  float diffuse = max(NdotL, 0.0);\n"
"  float specular = pow(NdotH,m);\n"
"  if(NdotL < 0.0 || NdotH < 0.0)\n"
"  	specular = 0.0;\n"
"  return vec4(ambient, diffuse, specular, 1.0);\n"
"}\n"
"\n"
"\n"
"void main()\n"
"{\n"
" // Create matrix for the quadric equation of the sphere\n"
" vec4 colonne1, colonne2, colonne3, colonne4;\n"
" mat4 mat;\n"
" vec4 equation = vec4(1.0,1.0,1.0,radius*radius);\n"
"\n"
"\n"
" colonne1 = vec4(equation.x,0.0,0.0,-equation.x*sphereposition.x);\n"
" colonne2 = vec4(0.0,equation.y,0.0,-equation.y*sphereposition.y);\n"
" colonne3 = vec4(0.0,0.0,equation.z,-equation.z*sphereposition.z);\n"
" colonne4 = vec4(-equation.x*sphereposition.x,\n"
"				 -equation.y*sphereposition.y,\n"
"				 -equation.z*sphereposition.z,\n"
"				 -equation.w +  equation.x*sphereposition.x*sphereposition.x + equation.y*sphereposition.y*sphereposition.y +equation.z*sphereposition.z*sphereposition.z);\n"
"\n"
" mat = mat4(colonne1,colonne2,colonne3,colonne4);\n"
"\n"
" // Ray calculation using near and far\n"
" Ray ray = primary_ray(i_near,i_far) ;\n"
" // Intersection between ray and surface for each pixel\n"
" vec3 M;\n"
" M = isect_surf(ray, mat);\n"
"\n"
" // Recalculate the depth in function of the new pixel position\n"
" gl_FragDepth = update_z_buffer(M, gl_ModelViewProjectionMatrix) ;\n"
"\n"
"\n"
"  // Transform normal to model space to view-space\n"
"  vec4 M1 = vec4(M,1.0);\n"
"  vec4 M2 =  (mat*M1);\n"
"  vec3 normal = normalize((gl_ModelViewMatrixInverseTranspose*M2).xyz);\n"
"\n"
"  // Give light vector position perpendicular to the screen\n"
"  vec3 lightvec = normalize(vec3(0.0,0.0,1.2));\n"
"  vec3 eyepos = vec3(0.0,0.0,1.0);\n"
"\n"
"  // calculate half-angle vector\n"
"  vec3 halfvec = normalize(lightvec + eyepos);\n"
"\n"
"  // Parameters used to calculate per pixel lighting\n"
"  // see http://http.developer.nvidia.com/CgTutorial/cg_tutorial_chapter05.html\n"
"\n"
"  float shininess = 0.5;\n"
"  float diffuse = dot(normal,lightvec);\n"
"  float specular = dot(halfvec, normal);\n"
"  vec4 lighting = lit(diffuse, specular, 32.0) ;\n"
"\n"
"  vec3 diffusecolor = color.xyz;\n"
"  vec3 specularcolor = vec3(1.0,1.0,1.0);\n"
"\n"
"  // Give color parameters to the Graphic card\n"
"  gl_FragColor.rgb = lighting.y * diffusecolor + lighting.z * specularcolor;\n"
"  gl_FragColor.a = 1.0;\n"
"\n"
"\n"
"  	// ############## Fog effect #####################################################\n"
"	// To use fog comment the two previous lines: ie  gl_FragColor.rgb = � and   gl_FragColor.a = 1.0;\n"
"	// and uncomment the next lines.\n"
"	// Color of the fog: white\n"
"	//float fogDistance  = update_z_buffer(M, gl_ModelViewMatrix) ;\n"
"  	//float fogExponent  = fogDistance * fogDistance * 0.007;\n"
"	//vec3 fogColor   = vec3(1.0, 1.0, 1.0);\n"
"	//float fogFactor   = exp2(-abs(fogExponent));\n"
"	//fogFactor = clamp(fogFactor, 0.0, 1.0);\n"
"\n"
"	//vec3 final_color = lighting.y * diffusecolor + lighting.z * specularcolor;\n"
"	//gl_FragColor.rgb = mix(fogColor,final_color,fogFactor);\n"
"  	//gl_FragColor.a = 1.0;\n"
"	// ##################################################################################\n"
"\n"
"\n"
"}\n"
;
#endif // __BALLIMPROVED_FRAG_H__
