//
// Created by aelliixx on 2023-11-11.
//

#pragma once
#include <string>
#include <variant>

namespace alx {
class Target
{
public:
	enum class ArchType
	{
		X86_64
	};
	enum class OSType
	{
		Linux
	};
	enum class ObjectFormatType
	{
		ELF
	};

private:
	ArchType m_arch{};
	OSType m_os{};
	ObjectFormatType m_objectFormat{};

public:
	Target() = default;
	Target(const ArchType arch, const OSType os, const ObjectFormatType objectFormat)
	  : m_arch(arch),
		m_os(os),
		m_objectFormat(objectFormat)
	{}

	[[nodiscard]] ArchType Arch() const { return m_arch; }
	[[nodiscard]] OSType OS() const { return m_os; }
	[[nodiscard]] ObjectFormatType ObjectFormat() const { return m_objectFormat; }
	[[nodiscard]] static std::string TargetToString(const std::variant<ArchType, OSType, ObjectFormatType>&);
};
}
