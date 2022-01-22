xof 0303txt 0032
template ColorRGBA {
 <35ff44e0-6c7c-11cf-8f52-0040333594a3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
 FLOAT alpha;
}

template ColorRGB {
 <d3e16e81-7835-11cf-8f52-0040333594a3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
}

template Material {
 <3d82ab4d-62da-11cf-ab39-0020af71e433>
 ColorRGBA faceColor;
 FLOAT power;
 ColorRGB specularColor;
 ColorRGB emissiveColor;
 [...]
}

template TextureFilename {
 <a42790e1-7810-11cf-8f52-0040333594a3>
 STRING filename;
}

template Frame {
 <3d82ab46-62da-11cf-ab39-0020af71e433>
 [...]
}

template Matrix4x4 {
 <f6f23f45-7686-11cf-8f52-0040333594a3>
 array FLOAT matrix[16];
}

template FrameTransformMatrix {
 <f6f23f41-7686-11cf-8f52-0040333594a3>
 Matrix4x4 frameMatrix;
}

template Vector {
 <3d82ab5e-62da-11cf-ab39-0020af71e433>
 FLOAT x;
 FLOAT y;
 FLOAT z;
}

template MeshFace {
 <3d82ab5f-62da-11cf-ab39-0020af71e433>
 DWORD nFaceVertexIndices;
 array DWORD faceVertexIndices[nFaceVertexIndices];
}

template Mesh {
 <3d82ab44-62da-11cf-ab39-0020af71e433>
 DWORD nVertices;
 array Vector vertices[nVertices];
 DWORD nFaces;
 array MeshFace faces[nFaces];
 [...]
}

template MeshNormals {
 <f6f23f43-7686-11cf-8f52-0040333594a3>
 DWORD nNormals;
 array Vector normals[nNormals];
 DWORD nFaceNormals;
 array MeshFace faceNormals[nFaceNormals];
}

template MeshMaterialList {
 <f6f23f42-7686-11cf-8f52-0040333594a3>
 DWORD nMaterials;
 DWORD nFaceIndexes;
 array DWORD faceIndexes[nFaceIndexes];
 [Material <3d82ab4d-62da-11cf-ab39-0020af71e433>]
}

template Coords2d {
 <f6f23f44-7686-11cf-8f52-0040333594a3>
 FLOAT u;
 FLOAT v;
}

template MeshTextureCoords {
 <f6f23f40-7686-11cf-8f52-0040333594a3>
 DWORD nTextureCoords;
 array Coords2d textureCoords[nTextureCoords];
}

template XSkinMeshHeader {
 <3cf169ce-ff7c-44ab-93c0-f78f62d172e2>
 WORD nMaxSkinWeightsPerVertex;
 WORD nMaxSkinWeightsPerFace;
 WORD nBones;
}

template SkinWeights {
 <6f0d123b-bad2-4167-a0d0-80224f25fabb>
 STRING transformNodeName;
 DWORD nWeights;
 array DWORD vertexIndices[nWeights];
 array FLOAT weights[nWeights];
 Matrix4x4 matrixOffset;
}


Material ho_in_decal_web_A01_3 {
 1.000000;1.000000;1.000000;1.000000;;
 43.219280;
 0.020000;0.020000;0.020000;;
 0.000000;0.000000;0.000000;;

 TextureFilename {
  "ugc_obj_webA_01.dds";
 }
}

Material ho_in_decal_web_A01_11 {
 1.000000;1.000000;1.000000;1.000000;;
 43.219280;
 0.020000;0.020000;0.020000;;
 0.000000;0.000000;0.000000;;

 TextureFilename {
  "ugc_obj_webA_01.dds";
 }
}

Frame Scene_Root {
 

 FrameTransformMatrix {
  0.393701,0.000000,0.000000,0.000000,0.000000,-0.000000,-0.393701,0.000000,0.000000,0.393701,-0.000000,0.000000,0.000000,0.000000,0.000000,1.000000;;
 }

 Frame ho_in_decal_web_A01 {
  

  FrameTransformMatrix {
   1.000000,0.000000,0.000000,0.000000,0.000000,-0.000000,-1.000000,0.000000,0.000000,1.000000,-0.000000,0.000000,0.000000,-0.000010,224.743270,1.000000;;
  }
 }

 Frame ho_in_decal_web_A02 {
  

  FrameTransformMatrix {
   1.000000,0.000000,0.000000,0.000000,0.000000,-0.000000,-1.000000,0.000000,0.000000,1.000000,-0.000000,0.000000,0.000000,0.009062,224.743270,1.000000;;
  }
 }
}

Frame ho_in_decal_web_A01_ {
 

 FrameTransformMatrix {
  0.393701,0.000000,0.000000,0.000000,0.000000,-0.000000,-0.393701,0.000000,0.000000,0.393701,-0.000000,0.000000,0.000000,0.000000,0.000000,1.000000;;
 }

 Mesh ho_in_decal_web_A01_ {
  4;
  224.999987;0.000000;-0.256760;,
  -224.999987;0.000000;-0.256760;,
  224.999987;-0.000020;449.74323;,
  -224.999987;-0.000020;449.74323;;
  2;
  3;0,1,2;,
  3;3,2,1;;

  MeshNormals {
   4;
   0.000000;1.000000;0.000000;,
   0.000000;1.000000;0.000000;,
   0.000000;1.000000;0.000000;,
   0.000000;1.000000;0.000000;;
   2;
   3;0,1,2;,
   3;3,2,1;;
  }

  MeshMaterialList {
   1;
   2;
   0,
   0;
   { ho_in_decal_web_A01_3 }
  }

  MeshTextureCoords c1 {
   4;
   1.000000;1.000000;,
   0.000000;1.000000;,
   1.000000;0.000000;,
   0.000000;0.000000;;
  }

  XSkinMeshHeader {
   1;
   1;
   1;
  }

  SkinWeights {
   "ho_in_decal_web_A01";
   4;
   0,
   1,
   2,
   3;
   1.000000,
   1.000000,
   1.000000,
   1.000000;
   1.000000,0.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,-1.000000,0.000000,0.000000,0.000000,224.743245,0.000010,1.000000;;
  }
 }
}

Frame ho_in_decal_web_A02_ {
 

 FrameTransformMatrix {
  0.393701,0.000000,0.000000,0.000000,0.000000,-0.000000,-0.393701,0.000000,0.000000,0.393701,-0.000000,0.000000,0.000000,0.000000,0.000000,1.000000;;
 }

 Mesh ho_in_decal_web_A02_ {
  4;
  224.999987;0.009053;449.74323;,
  -224.999987;0.009072;-0.256760;,
  224.999987;0.009072;-0.256760;,
  -224.999987;0.009053;449.74323;;
  2;
  3;0,1,2;,
  3;1,0,3;;

  MeshNormals {
   4;
   0.000000;-1.000000;-0.000000;,
   0.000000;-1.000000;-0.000000;,
   0.000000;-1.000000;-0.000000;,
   0.000000;-1.000000;-0.000000;;
   2;
   3;0,1,2;,
   3;1,0,3;;
  }

  MeshMaterialList {
   1;
   2;
   0,
   0;
   { ho_in_decal_web_A01_11 }
  }

  MeshTextureCoords c1 {
   4;
   1.000000;0.000000;,
   0.000000;1.000000;,
   1.000000;1.000000;,
   0.000000;0.000000;;
  }

  XSkinMeshHeader {
   1;
   1;
   1;
  }

  SkinWeights {
   "ho_in_decal_web_A02";
   4;
   0,
   1,
   2,
   3;
   1.000000,
   1.000000,
   1.000000,
   1.000000;
   1.000000,0.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,-1.000000,0.000000,0.000000,0.000000,224.743245,-0.009062,1.000000;;
  }
 }
}