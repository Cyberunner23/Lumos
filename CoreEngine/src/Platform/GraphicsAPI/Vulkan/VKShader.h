#pragma once

#include "Dependencies/vulkan/vulkan.h"
#include "Graphics/API/Shader.h"

namespace jm
{
	namespace graphics
	{
		class VKShader : public Shader
		{
		public:
			VKShader(const std::string& shaderName, const std::string& filePath);
			~VKShader();

			bool Init();
			void Unload() const;

			VkPipelineShaderStageCreateInfo* GetShaderStages() const;
			uint32_t GetStageCount() const;

			void Bind() const {};
			void Unbind() const {};

			void SetSystemUniformBuffer(ShaderType type, byte* data, uint size, uint slot = 0) {};
			void SetUserUniformBuffer(ShaderType type, byte* data, uint size) {};

			const ShaderUniformBufferList GetSystemUniforms(ShaderType type) const { return ShaderUniformBufferList(); };
			const ShaderUniformBufferDeclaration* GetUserUniformBuffer(ShaderType type) const { return nullptr; };

			const std::vector<ShaderType> GetShaderTypes() const { return std::vector<ShaderType>(); };

			const String& GetName() const { return m_Name; }
			const String& GetFilePath() const { return m_FilePath; };

			static void PreProcess(const String& source, std::map<ShaderType, String>* sources);
			static void ReadShaderFile(std::vector<String> lines, std::map<ShaderType, String>* shaders);

		private:
			VkPipelineShaderStageCreateInfo* 	m_ShaderStages;
			uint32_t 							m_StageCount;
			String 								m_Name;
			String								m_FilePath;
			String 								m_Source;
			std::vector<ShaderType> 			m_ShaderTypes;
		};
	}
}