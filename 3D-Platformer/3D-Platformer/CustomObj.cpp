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

CustomObj::CustomObj(Graphics& gfx, std::wstring _modelName, float _x, float _y, float _z, float _scaleX, float _scaleY, float _scaleZ) :
	modelName(_modelName),
	xPos(_x),
	yPos(_y),
	zPos(_z)
{
	LoadObjModel(_modelName);

	if (!IsStaticInitialised())
	{
		//Calculate normals for flat object
		/*
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
		*/

		//Bind vertex buffer
		AddStaticBind(std::make_unique<VertexBuffer>(gfx, vertices));

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
		DirectX::XMMatrixScaling(_scaleX, _scaleY, _scaleZ)
	);

	std::vector<DirectX::XMFLOAT3> verticePositions;
	for (int i = 0; i < vertices.size(); i++)
	{
		verticePositions.push_back(vertices[i].pos);
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
	std::wstring modelPath = L"3DObjects\\";

	//Open file
	std::wifstream fileIn(modelPath + filename.c_str() + L".obj");	

	std::wstring meshMatLib;								

	//Arrays to store our model's information
	std::vector<DirectX::XMFLOAT3> vertPos;
	std::vector<DirectX::XMFLOAT2> vertTexCoord;
	std::vector<DirectX::XMFLOAT3> vertNorm;
	std::vector<std::wstring> meshMaterials;

	//Vertex definition indices
	std::vector<int> vertPosIndex;
	std::vector<int> vertNormIndex;
	std::vector<int> vertTCIndex;

	//Make sure we have a default if no tex coords or normals are defined
	bool hasTexCoord = false;
	bool hasNormals = false;

	//Temp variables to store into vectors
	std::wstring meshMaterialsTemp;
	int vertPosIndexTemp;
	int vertNormIndexTemp;
	int vertTCIndexTemp;

	
	wchar_t c;				//Current character to check against
	std::wstring face;		//Holds the string containing our face vertices
	int vIndex = 0;			//Keep track of our vertex index count
	int triangleCount = 0;	//Total Triangles
	int totalVerts = 0;
	int meshTriangles = 0;

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
						fileIn >> vtcu >> vtcv;		//Store next two types

						vertTexCoord.push_back(DirectX::XMFLOAT2(vtcu, vtcv));

						hasTexCoord = true;	//We know the model uses texture coords
					}

					//vn - vertex normals
					if (c == 'n')
					{
						float vnx, vny, vnz;
						fileIn >> vnx >> vny >> vnz;
					
						vertNorm.push_back(DirectX::XMFLOAT3(vnx, vny, vnz));

						hasNormals = true;	//We know the model defines normals
					}
					break;

				//Get Face Indexes
				case 'f':	
					
					//f - defines the faces
					c = fileIn.get();
					if (c == ' ')
					{
						face = L"";
						std::wstring vertexDefinition;	//Holds one vertex definition at a time
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

						std::wstringstream stringString(face);

						if (face.length() > 0)
						{
							//Holds the first and last vertice's index
							int firstVIndex, lastVIndex;	

							//First three vertices (first triangle)
							for (int i = 0; i < 3; ++i)		
							{
								//Get vertex definition (vPos/vTexCoord/vNorm)
								stringString >> vertexDefinition;	

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
												vertTCIndexTemp = 0;
											}
										}

										//Vertex Texture Coordinate
										else if (vetexSection == 1)	
										{
											//Check to see if there even is a tex coord
											if (vertPart != L"")	
											{
												wstringToInt >> vertTCIndexTemp;
												//Change array start position from 1 to 0
												vertTCIndexTemp -= 1;	
											}
											//If there is no texture coordinate, make a default
											else	
											{
												vertTCIndexTemp = 0;
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
											if (vertTCIndexTemp == vertTCIndex[iCheck])
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
									vertTCIndex.push_back(vertTCIndexTemp);
									vertNormIndex.push_back(vertNormIndexTemp);

									totalVerts++;	
									indices.push_back(totalVerts - 1);	//Set index for this vertex
								}

								//Set first vertex
								if (i == 0)
								{
									firstVIndex = indices[vIndex];
								}

								//If this was the last vertex in the first triangle, we will make sure the next triangle uses this one 
								if (i == 2)
								{
									lastVIndex = indices[vIndex];	//The last vertex index of this TRIANGLE
								}
								vIndex++;	//Increment index count
							}

							meshTriangles++;	//One triangle down

							//MAYBE DELETE?

							//If there are more than three vertices in the face definition, we need to make sure
							//we convert the face to triangles. We created our first triangle above, now we will
							//create a new triangle for every new vertex in the face, using the very first vertex
							//of the face, and the last vertex from the triangle before the current triangle
							for (int l = 0; l < triangleCount - 1; ++l)	//Loop through the next vertices to create new triangles
							{
								//First vertex of this triangle (the very first vertex of the face too)
								indices.push_back(firstVIndex);			//Set index for this vertex
								vIndex++;

								//Second Vertex of this triangle (the last vertex used in the tri before this one)
								indices.push_back(lastVIndex);			//Set index for this vertex
								vIndex++;

								//Get the third vertex for this triangle
								stringString >> vertexDefinition;

								std::wstring vertPart;
								int whichPart = 0;

								//Parse this string (same as above)
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
												vertTCIndexTemp = 0;
												vertNormIndexTemp = 0;
											}
										}
										else if (whichPart == 1)
										{
											if (vertPart != L"")
											{
												wstringToInt >> vertTCIndexTemp;
												vertTCIndexTemp -= 1;
											}
											else
											{
												vertTCIndexTemp = 0;
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
								if (totalVerts >= 3)	//Make sure we at least have one triangle to check
								{
									for (int iCheck = 0; iCheck < totalVerts; ++iCheck)
									{
										if (vertPosIndexTemp == vertPosIndex[iCheck] && !vertAlreadyExists)
										{
											if (vertTCIndexTemp == vertTCIndex[iCheck])
											{
												indices.push_back(iCheck);			//Set index for this vertex
												vertAlreadyExists = true;		//If we've made it here, the vertex already exists
											}
										}
									}
								}

								if (!vertAlreadyExists)
								{
									vertPosIndex.push_back(vertPosIndexTemp);
									vertTCIndex.push_back(vertTCIndexTemp);
									vertNormIndex.push_back(vertNormIndexTemp);
									totalVerts++;					//New vertex created, add to total verts
									indices.push_back(totalVerts - 1);		//Set index for this vertex
								}

								//Set the second vertex for the next triangle to the last vertex we got		
								lastVIndex = indices[vIndex];	//The last vertex index of this TRIANGLE

								meshTriangles++;	//New triangle defined
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
											fileIn >> meshMatLib;
										}
									}
								}
							}
						}
					}

					break;

				case 'u':	//usemtl - which material to use
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
											meshMaterialsTemp = L"";	//Make sure this is cleared

											fileIn >> meshMaterialsTemp; //Get next type (string)

											meshMaterials.push_back(meshMaterialsTemp);
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

	//Make sure we have a default for the tex coord and normal
	//if one or both are not specified
	if (!hasNormals)
	{
		vertNorm.push_back(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	}
	if (!hasTexCoord)
	{
		vertTexCoord.push_back(DirectX::XMFLOAT2(0.0f, 0.0f));
	}

	//Close the obj file, and open the mtl file
	fileIn.close();
	fileIn.open(modelPath + meshMatLib.c_str());

	std::wstring lastStringRead;
	int matCount = material.size();	//total materials

	//kdset - If our diffuse color was not set, we can use the ambient color (which is usually the same)
	//If the diffuse color WAS set, then we don't need to set our diffuse color to ambient
	bool kdset = false;

	if (fileIn)
	{
		while (fileIn)
		{
			c = fileIn.get();	//Get next char

			switch (c)
			{
				//Check for comment
			case '#':
				c = fileIn.get();
				while (c != '\n')
					c = fileIn.get();
				break;

				//Set diffuse color
			case 'K':
				c = fileIn.get();
				if (c == 'd')	//Diffuse Color
				{
					c = fileIn.get();	//remove space

					fileIn >> material[matCount - 1].difColor.x;
					fileIn >> material[matCount - 1].difColor.y;
					fileIn >> material[matCount - 1].difColor.z;

					kdset = true;
				}

				//Ambient Color (We'll store it in diffuse if there isn't a diffuse already)
				if (c == 'a')
				{
					c = fileIn.get();	//remove space
					if (!kdset)
					{
						fileIn >> material[matCount - 1].difColor.x;
						fileIn >> material[matCount - 1].difColor.y;
						fileIn >> material[matCount - 1].difColor.z;
					}
				}
				break;

				//Check for transparency
			case 'T':
				c = fileIn.get();
				if (c == 'r')
				{
					c = fileIn.get();	//remove space
					float Transparency;
					fileIn >> Transparency;

					material[matCount - 1].difColor.w = Transparency;

					if (Transparency > 0.0f)
					{
						material[matCount - 1].transparent = true;
					}
				}
				break;

				//Some obj files specify d for transparency
			case 'd':
				c = fileIn.get();
				if (c == ' ')
				{
					float Transparency;
					fileIn >> Transparency;

					//'d' - 0 being most transparent, and 1 being opaque, opposite of Tr
					Transparency = 1.0f - Transparency;

					material[matCount - 1].difColor.w = Transparency;

					if (Transparency > 0.0f)
					{
						material[matCount - 1].transparent = true;
					}
				}
				break;

			//Get the diffuse map (texture)
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
									//TEXTURE NAME
									std::wstring fileNamePath;

									//Remove whitespace between map_Kd and file
									fileIn.get();	

									//Get the file path - We read the pathname char by char since
									//pathnames can sometimes contain spaces, so we will read until
									//we find the file extension
									bool texFilePathEnd = false;
									while (!texFilePathEnd)
									{
										c = fileIn.get();

										fileNamePath += c;

										if (c == '.')
										{
											for (int i = 0; i < 3; ++i)
											{
												fileNamePath += fileIn.get();
											}

											texFilePathEnd = true;
										}
									}

									textureName = fileNamePath;
								}
							}
							//map_d - alpha map
							else if (c == 'd')
							{
								//Alpha maps are usually the same as the diffuse map
								//So we will assume that for now by only enabling
								//transparency for this material, as we will already
								//be using the alpha channel in the diffuse map
								material[matCount - 1].transparent = true;
							}
						}
					}
				}
				break;

			case 'n':	//newmtl - Declare new material
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
										//New material, set its defaults
										SurfaceMaterial tempMat;
										material.push_back(tempMat);
										fileIn >> material[matCount].matName;
										matCount++;
										kdset = false;
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

	Vertex tempVert;

	//Create our vertices using the information we got 
	//from the file and store them in a vector
	for (int j = 0; j < totalVerts; ++j)
	{
		tempVert.pos = vertPos[vertPosIndex[j]];
		tempVert.normal = vertNorm[vertNormIndex[j]];
		tempVert.texCoord = vertTexCoord[vertTCIndex[j]];

		vertices.push_back(tempVert);
	}
}
