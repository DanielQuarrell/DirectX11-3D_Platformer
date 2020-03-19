#include "CustomObj.h"
//Bindables
#include "ConstantBuffers.h"
#include "IndexBuffer.h"
#include "InputLayout.h"
#include "PixelShader.h"
#include "Topology.h"
#include "TransformCbuf.h"
#include "VertexBuffer.h"
#include "VertexShader.h"
#include "Texture.h"

CustomObj::CustomObj(Graphics& gfx, std::wstring _modelName, float _x, float _y, float _z, float _yRot, float _xScale, float _yScale, float _zScale, bool _hasTexture, bool _hasLighting) :
	modelName(_modelName)
{
	xPos = _x;
	yPos = _y;
	zPos = _z;
	yRot = _yRot;
	xScale = _xScale;
	yScale = _yScale;
	zScale = _zScale;

	if (!IsStaticInitialised())
	{
		LoadObjModel(_modelName);

		//Calculate normals if Obj doesn't contain them
		if (!hasNormals)
		{
			for (size_t i = 0; i < indices.size(); i += 3)
			{
				Vertex& v0 = vertices[indices[i]];
				Vertex& v1 = vertices[indices[i + 1]];
				Vertex& v2 = vertices[indices[i + 2]];
				const DirectX::XMVECTOR p0 = DirectX::XMLoadFloat3(&v0.pos);
				const DirectX::XMVECTOR p1 = DirectX::XMLoadFloat3(&v1.pos);
				const DirectX::XMVECTOR p2 = DirectX::XMLoadFloat3(&v2.pos);

				const DirectX::XMVECTOR n = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMVectorSubtract(p1, p0), DirectX::XMVectorSubtract(p2, p0)));

				XMStoreFloat3(&v0.normal, n);
				XMStoreFloat3(&v1.normal, n);
				XMStoreFloat3(&v2.normal, n);
			}
		}

		//Bind vertex buffer
		AddStaticBind(std::make_unique<VertexBuffer>(gfx, vertices));

		if (_hasTexture)
		{
			//Bind texture from file
			std::wstring imagePath = L"Images\\";
			AddStaticBind(std::make_unique<Texture>(gfx, imagePath + textureName));

			//Create vertex shader
			auto pVertexShader = std::make_unique<VertexShader>(gfx, L"TextureVS.cso");
			auto pVertexShaderBytecode = pVertexShader->GetBytecode();
			AddStaticBind(std::move(pVertexShader));

			//Create pixel shader
			AddStaticBind(std::make_unique<PixelShader>(gfx, L"TexturePS.cso"));

			//Bind index buffer
			AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, indices));

			//Define Input layout
			const std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDesc =
			{
				{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
				{ "TexCoord",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
				{ "Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
			};

			//Bind Input vertex layout
			AddStaticBind(std::make_unique<InputLayout>(gfx, inputElementDesc, pVertexShaderBytecode));
		}
		else
		{
			//Calculate normals if Obj doesn't contain them

			if (!hasNormals)
			{
				for (size_t i = 0; i < indices.size(); i += 3)
				{
					Vertex& v0 = vertices[indices[i]];
					Vertex& v1 = vertices[indices[i + 1]];
					Vertex& v2 = vertices[indices[i + 2]];
					const DirectX::XMVECTOR p0 = DirectX::XMLoadFloat3(&v0.pos);
					const DirectX::XMVECTOR p1 = DirectX::XMLoadFloat3(&v1.pos);
					const DirectX::XMVECTOR p2 = DirectX::XMLoadFloat3(&v2.pos);

					const DirectX::XMVECTOR n = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMVectorSubtract(p1, p0), DirectX::XMVectorSubtract(p2, p0)));

					XMStoreFloat3(&v0.normal, n);
					XMStoreFloat3(&v1.normal, n);
					XMStoreFloat3(&v2.normal, n);
				}
			}

			//Create vertex shader
			auto pVertexShader = std::make_unique<VertexShader>(gfx, L"ColorIndexVS.cso");
			auto pVertexShaderBytecode = pVertexShader->GetBytecode();
			AddStaticBind(std::move(pVertexShader));

			if (_hasLighting)
			{
				//Create pixel shader
				AddStaticBind(std::make_unique<PixelShader>(gfx, L"ColorIndexPS.cso"));
			}
			else
			{
				//Create pixel shader
				AddStaticBind(std::make_unique<PixelShader>(gfx, L"ColorIndex2PS.cso"));
			}



			AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, indices));
			
			struct colour
			{
				float r;
				float g;
				float b;
				float a;
			};

			struct ConstantBuffer2
			{
				colour face_colors[3000];
			};

			std::vector<colour> face_colors_override;
			
			for (size_t i = 0; i < surfaceMaterials.size(); i++)
			{
				for (size_t j = 0; j < materials.size(); j++)
				{
					if (surfaceMaterials[i].matName == materials[j].matName)
					{
						for (int f = 0; f < surfaceMaterials[i].numOfFaces; f++)
						{
							if (materials[j].difColor.w < 0)
							{
								materials[j].difColor.w = 1.0f;
							}

							face_colors_override.push_back({ materials[j].difColor.x, materials[j].difColor.y, materials[j].difColor.z, materials[j].difColor.w });
						}
					}
				}
			}

			ConstantBuffer2 cb2;
			std::copy(face_colors_override.begin(), face_colors_override.end(), cb2.face_colors);
			
			//Bind Constant buffer to the pixel shader
			AddStaticBind(std::make_unique<PixelConstantBuffer<ConstantBuffer2>>(gfx, cb2));

			//Define Input layout
			const std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDesc =
			{
				{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
				{ "Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
			};

			//Bind Input vertex layout
			AddStaticBind(std::make_unique<InputLayout>(gfx, inputElementDesc, pVertexShaderBytecode));
		}

		//Set primitive topology to triangle list
		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else
	{
		SetIndexFromStatic();
	}

	//Bind transform buffer
	AddBind(std::make_unique<TransformCbuf>(gfx, *this));

	//Model deformation transform (Instance)
	DirectX::XMStoreFloat3x3(
		&modelTransform,
		DirectX::XMMatrixScaling(xScale, yScale, zScale)
	);

	std::vector<DirectX::XMFLOAT3> verticePositions;
	for (int v = 0; v < vertices.size(); v++)
	{
		verticePositions.push_back(vertices[v].pos);
	}

	CreateBoundingBox(verticePositions);
	CalculateAABB(GetTransformXM());
}

void CustomObj::SetPosition(float _x, float _y, float _z)
{
	xPos = _x;
	yPos = _y;
	zPos = _z;
}

void CustomObj::Update(float dt) noexcept
{
}

DirectX::XMMATRIX CustomObj::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat3x3(&modelTransform) *
		DirectX::XMMatrixRotationRollPitchYaw(xRot, yRot, zRot) *
		DirectX::XMMatrixTranslation(xPos, yPos, zPos);
}

void CustomObj::LoadObjModel(std::wstring filename)
{
	//Obj path
	std::wstring objectPath = L"3DObjects\\";

	//Open file
	std::wifstream fileIn(objectPath + filename.c_str() + L".obj");	

	//Mtl file name
	std::wstring mtl_filename;								

	//Arrays to store model information
	std::vector<DirectX::XMFLOAT3> vertPos;
	std::vector<DirectX::XMFLOAT2> vertTexCoord;
	std::vector<DirectX::XMFLOAT3> vertNorm;

	//Vertex definition indices
	std::vector<int> vertPosIndex;
	std::vector<int> vertNormIndex;
	std::vector<int> vertTexCoordIndex;

	//Temp variables to store into vectors
	int vertPosIndexTemp;
	int vertNormIndexTemp;
	int vertTexCoordIndexTemp;


	wchar_t c;				//Current character to check against
	std::wstring face;		//Holds the string containing our face vertices
	int vIndex = 0;			//Keep track of our vertex index count
	int triangleCount = 0;	//Total Triangles
	int totalVerts = 0;		//Total Verticies

	//total surface materials
	int surfaceMatCount = 0;

	//Check to see if the file was opened
	if (fileIn)
	{
		while (fileIn)
		{
			c = fileIn.get();	//Get next char

			switch (c)
			{
				//Ignore comments
				case '#':
					c = fileIn.get();
					while (c != '\n')
						c = fileIn.get();
					break;
				
				//Get Vertex Descriptions
				case 'v':	
					c = fileIn.get();

					//v - vert position
					if (c == ' ')	
					{
						float vz, vy, vx;
						fileIn >> vx >> vy >> vz;	//Store the next three types

						vertPos.push_back(DirectX::XMFLOAT3(vx, vy, vz));
					}

					//vt - vertex texture coordinates
					else if (c == 't')	
					{
						float vtcu, vtcv;
						fileIn >> vtcu >> vtcv;

						vertTexCoord.push_back(DirectX::XMFLOAT2(vtcu, vtcv));

						//Model uses texture
						hasTexture = true;	
					}

					//vn - vertex normals
					if (c == 'n')
					{
						float vnx, vny, vnz;
						fileIn >> vnx >> vny >> vnz;
					
						vertNorm.push_back(DirectX::XMFLOAT3(vnx, vny, vnz));

						//Model difines normals
						hasNormals = true;	
					}
					break;

				case 'u':
					c = fileIn.get();
					if (c == 's')
					{
						c = fileIn.get();
						if (c == 'e')
						{
							c = fileIn.get();
							if (c == 'm')
							{
								c = fileIn.get();
								if (c == 't')
								{
									c = fileIn.get();
									if (c == 'l')
									{
										c = fileIn.get();
										if (c == ' ')
										{
											//Define material to use for the next set of faces
											SurfaceMaterial tempSurfaceMat;
											surfaceMaterials.push_back(tempSurfaceMat);
											fileIn >> surfaceMaterials[surfaceMatCount].matName;
											surfaceMaterials[surfaceMatCount].numOfFaces = 0;

											surfaceMatCount++;
										}
									}
								}
							}
						}
					}
					break;

				//Get Face Indexes
				case 'f':	
					if (surfaceMaterials.size() != 0)
					{
						//Number of faces using this material
						surfaceMaterials[surfaceMatCount - 1].numOfFaces++;
					}

					totalFaces++;

					//f - defines the faces
					c = fileIn.get();
					if (c == ' ')
					{
						face = L"";

						//Holds one vertex definition at a time
						std::wstring vertexDefinition;	
						triangleCount = 0;

						c = fileIn.get();

						//While the character is not a new line
						while (c != '\n')
						{
							//Add the character to the face string
							face += c;

							//If the next character is a space, increase the triangle count
							c = fileIn.get();	
							if (c == ' ')		
							{
								triangleCount++;		
							}
						}

						//Remove any extra triangles created from spaces at the end of the string
						if (face[face.length() - 1] == ' ')
						{
							triangleCount--;
						}

						//Every vertex in the face after the first two are new faces
						triangleCount -= 1;		

						std::wstringstream stringStream(face);

						if (face.length() > 0)
						{
							//Holds the first and last vertice's index
							int firstVIndex, lastVIndex;	

							//First three vertices (first triangle)
							for (int i = 0; i < 3; ++i)		
							{
								//Get vertex definition (vPos/vTexCoord/vNorm)
								stringStream >> vertexDefinition;	

								//(vPos, vTexCoord, or vNorm)
								std::wstring vertPart;
								int vetexSection = 0;		

								//Parse this stringVertex
								for (int j = 0; j < vertexDefinition.length(); ++j)
								{
									if (vertexDefinition[j] != '/')	//If there is no divider "/", add a char to our vertPart
									{
										vertPart += vertexDefinition[j];
									}

									//If the current char is a divider "/", or its the last character in the string
									if (vertexDefinition[j] == '/' || j == vertexDefinition.length() - 1)
									{
										std::wistringstream wstringToInt(vertPart);	//Used to convert wstring to int

										//Vertex position
										if (vetexSection == 0)
										{
											wstringToInt >> vertPosIndexTemp;
											//Change array start position from 1 to 0
											vertPosIndexTemp -= 1;		

											//Check to see if the vert pos was the only thing specified
											if (j == vertexDefinition.length() - 1)
											{
												vertNormIndexTemp = 0;
												vertTexCoordIndexTemp = 0;
											}
										}

										//Vertex Texture Coordinate
										else if (vetexSection == 1)	
										{
											//Check to see if there even is a tex coord
											if (vertPart != L"")	
											{
												wstringToInt >> vertTexCoordIndexTemp;
												//Change array start position from 1 to 0
												vertTexCoordIndexTemp -= 1;	
											}
											//If there is no texture coordinate, make a default
											else	
											{
												vertTexCoordIndexTemp = 0;
											}

											//If the current char is the second to last, there is no normal
											if (j == vertexDefinition.length() - 1)
											{
												vertNormIndexTemp = 0;
											}
										}

										//VertexNormal
										else if (vetexSection == 2)	
										{
											std::wistringstream wstringToInt(vertPart);

											wstringToInt >> vertNormIndexTemp;
											//Change array start position from 1 to 0
											vertNormIndexTemp -= 1;		
										}

										//Clear string
										vertPart = L"";

										//Move on to next vertex	
										vetexSection++;					
									}
								}

								//Avoid duplicate vertices
								bool vertexExists = false;
								//Make sure we at least have one triangle to check
								if (totalVerts >= 3)	
								{
									//Loop through all the vertices
									for (int iCheck = 0; iCheck < totalVerts; ++iCheck)
									{
										//Check if it has already been loaded
										if (vertPosIndexTemp == vertPosIndex[iCheck] && !vertexExists)
										{
											if (vertTexCoordIndexTemp == vertTexCoordIndex[iCheck])
											{
												indices.push_back(iCheck);	//Set index for this vertex
												vertexExists = true;		
											}
										}
									}
								}

								//Add vertex if not added already
								if (!vertexExists)
								{
									vertPosIndex.push_back(vertPosIndexTemp);
									vertTexCoordIndex.push_back(vertTexCoordIndexTemp);
									vertNormIndex.push_back(vertNormIndexTemp);

									totalVerts++;	
									indices.push_back(totalVerts - 1);	
								}

								//Set first vertex
								if (i == 0)
								{
									firstVIndex = indices[vIndex];
								}

								//If this was the last vertex in the triangle, make sure the next triangle uses this one 
								if (i == 2)
								{
									//The last vertex index of the current triangle
									lastVIndex = indices[vIndex];	
								}
								
								//Increment index count
								vIndex++;	
							}

							//Create new triangles for every new vertex in the face
							for (int l = 0; l < triangleCount - 1; l++)
							{
								//Set index for first vertex
								indices.push_back(firstVIndex);            
								vIndex++;

								//Set index for second vertex (Last index of previous triangle)
								indices.push_back(lastVIndex);           
								vIndex++;

								//Get third vertex for this triangle
								stringStream >> vertexDefinition;

								std::wstring vertPart;
								int whichPart = 0;

								//Parse string 
								for (int j = 0; j < vertexDefinition.length(); ++j)
								{
									if (vertexDefinition[j] != '/')
									{
										vertPart += vertexDefinition[j];
									}
									if (vertexDefinition[j] == '/' || j == vertexDefinition.length() - 1)
									{
										std::wistringstream wstringToInt(vertPart);

										if (whichPart == 0)
										{
											wstringToInt >> vertPosIndexTemp;
											vertPosIndexTemp -= 1;

											//Check to see if the vert pos was the only thing specified
											if (j == vertexDefinition.length() - 1)
											{
												vertTexCoordIndexTemp = 0;
												vertNormIndexTemp = 0;
											}
										}
										else if (whichPart == 1)
										{
											if (vertPart != L"")
											{
												wstringToInt >> vertTexCoordIndexTemp;
												vertTexCoordIndexTemp -= 1;
											}
											else
											{
												vertTexCoordIndexTemp = 0;
											}
											if (j == vertexDefinition.length() - 1)
											{
												vertNormIndexTemp = 0;
											}
										}
										else if (whichPart == 2)
										{
											std::wistringstream wstringToInt(vertPart);

											wstringToInt >> vertNormIndexTemp;
											vertNormIndexTemp -= 1;
										}

										vertPart = L"";
										whichPart++;
									}
								}

								//Check for duplicate vertices
								bool vertAlreadyExists = false;
								//Make sure a triangle exists to check
								if (totalVerts >= 3)    
								{
									for (int iCheck = 0; iCheck < totalVerts; ++iCheck)
									{
										if (vertPosIndexTemp == vertPosIndex[iCheck] && !vertAlreadyExists)
										{
											if (vertTexCoordIndexTemp == vertTexCoordIndex[iCheck])
											{
												//Set index for this vertex
												indices.push_back(iCheck);            
												vertAlreadyExists = true; 
											}
										}
									}
								}

								if (!vertAlreadyExists)
								{
									vertPosIndex.push_back(vertPosIndexTemp);
									vertTexCoordIndex.push_back(vertTexCoordIndexTemp);
									vertNormIndex.push_back(vertNormIndexTemp);

									totalVerts++;              
									//Set index for this vertex
									indices.push_back(totalVerts - 1);        
								}

								//Set the second vertex for the next triangle      
								lastVIndex = indices[vIndex];

								vIndex++;
							}
						}
					}
					break;

				case 'm':	//mtllib - material library filename
					c = fileIn.get();
					if (c == 't')
					{
						c = fileIn.get();
						if (c == 'l')
						{
							c = fileIn.get();
							if (c == 'l')
							{
								c = fileIn.get();
								if (c == 'i')
								{
									c = fileIn.get();
									if (c == 'b')
									{
										c = fileIn.get();
										if (c == ' ')
										{
											//Store the material libraries file name
											fileIn >> mtl_filename;
										}
									}
								}
							}
						}
					}
					break;

				default:
					break;
			}
		}
	}

	//Create default for the tex coord and normal
	if (!hasNormals)
	{
		vertNorm.push_back(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	}
	if (!hasTexture)
	{
		vertTexCoord.push_back(DirectX::XMFLOAT2(0.0f, 0.0f));
	}

	//Close the obj file, and open the mtl file
	fileIn.close();
	fileIn.open(objectPath + mtl_filename.c_str());

	//total materials
	int matCount = 0;

	//kdset - If diffuse colour was not set, use the ambient colour
	bool kdSet = false;

	if (fileIn)
	{
		while (fileIn)
		{
			c = fileIn.get();	//Get next char

			switch (c)
			{
				//Check for comments
				case '#':
					c = fileIn.get();
					while (c != '\n')
					{
						c = fileIn.get();
					}
					break;

				//Set diffuse color
				case 'K':
					c = fileIn.get();

					//Diffuse Color
					if (c == 'd')	
					{
						//remove space
						c = fileIn.get();	

						fileIn >> materials[matCount - 1].difColor.x;
						fileIn >> materials[matCount - 1].difColor.y;
						fileIn >> materials[matCount - 1].difColor.z;

						kdSet = true;
					}

					//Ambient Color
					if (c == 'a')
					{
						//remove space
						c = fileIn.get();	
						if (!kdSet)
						{
							fileIn >> materials[matCount - 1].difColor.x;
							fileIn >> materials[matCount - 1].difColor.y;
							fileIn >> materials[matCount - 1].difColor.z;
						}
					}
					break;

				//Check for transparency
				case 'T':
					c = fileIn.get();
					if (c == 'r')
					{
						//Remove space
						c = fileIn.get();

						float Transparency;
						fileIn >> Transparency;

						materials[matCount - 1].difColor.w = Transparency;

						if (Transparency > 0.0f)
						{
							materials[matCount - 1].transparent = true;
						}
					}
					break;

				//d is also used for transparancy
				case 'd':
					c = fileIn.get();
					if (c == ' ')
					{
						float Transparency;
						fileIn >> Transparency;

						//'d' - 0 being most transparent, and 1 being opaque, opposite of Tr
						Transparency = 1.0f - Transparency;

						materials[matCount - 1].difColor.w = Transparency;

						if (Transparency > 0.0f)
						{
							materials[matCount - 1].transparent = true;
						}
					}
					break;

				//Get the texture or difuse map
				case 'm':
					c = fileIn.get();
					if (c == 'a')
					{
						c = fileIn.get();
						if (c == 'p')
						{
							c = fileIn.get();
							if (c == '_')
							{
								//map_Kd - Diffuse map
								c = fileIn.get();
								if (c == 'K')
								{
									c = fileIn.get();
									if (c == 'd')
									{
										//Store texture name in a string
										std::wstring textureString;

										//Remove whitespace between map_Kd and fileName
										fileIn.get();	

										//Get texture name and file extension
										do
										{
											c = fileIn.get();

											textureString += c;

											//Read in file extension
											if (c == '.')
											{
												for (int i = 0; i < 3; ++i)
												{
													textureString += fileIn.get();
												}
											}
										} while (c != '.');

										textureName = textureString;
									}
								}

								//map_d - Alpha map
								else if (c == 'd')
								{
									//If mtl has an alpha map, enable transparency for difuse map
									materials[matCount - 1].transparent = true;
								}
							}
						}
					}
					break;

				//newmtl - Declare new material
				case 'n':	
					c = fileIn.get();
					if (c == 'e')
					{
						c = fileIn.get();
						if (c == 'w')
						{
							c = fileIn.get();
							if (c == 'm')
							{
								c = fileIn.get();
								if (c == 't')
								{
									c = fileIn.get();
									if (c == 'l')
									{
										c = fileIn.get();
										if (c == ' ')
										{
											//Create new material, set its defaults
											Material tempMat;
											materials.push_back(tempMat);
											fileIn >> materials[matCount].matName;
											matCount++;
											kdSet = false;
										}
									}
								}
							}
						}
					}
					break;

				default:
					break;
				}
			}
	}

	//Create vertices from file
	for (int j = 0; j < totalVerts; ++j)
	{
		Vertex newVert;

		newVert.pos = vertPos[vertPosIndex[j]];
		newVert.texCoord = vertTexCoord[vertTexCoordIndex[j]];
		newVert.normal = vertNorm[vertNormIndex[j]];

		vertices.push_back(newVert);
	}
}
