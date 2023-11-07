/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-11-07.
//

#pragma once

#include <string>
#include <vector>
namespace alx {

class FilePath
{
	std::string m_name;
	std::vector<std::string> m_path;
	std::string m_full_path;
	std::string m_extension;
	std::string m_name_without_extension;

public:
	FilePath() = default;
	
	explicit FilePath(const std::string& name)
	{
		m_full_path = name;
		
		// If the file has an extension, then the extension is everything after the last dot.
		if (name.find('.') != std::string::npos) {
			auto lastDot = name.find_last_of('.');
			m_extension = name.substr(lastDot + 1);
		}
		
		// If the file is in the current directory, then the path is empty.
		if (name.find('/') == std::string::npos) {
			m_name = name;
			m_name_without_extension = m_name.substr(0, m_name.find_last_of('.'));
			return;
		}
		auto lastSlash = name.find_last_of('/');
		m_name = name.substr(lastSlash + 1);
		m_name_without_extension = m_name.substr(0, m_name.find_last_of('.'));
		auto path = name.substr(0, lastSlash);
		auto slash = path.find('/');
		while (slash != std::string::npos) {
			m_path.emplace_back(path.substr(0, slash));
			path = path.substr(slash + 1);
			slash = path.find('/');
		}
	}
	
	[[nodiscard]] const std::string& GetName() const { return m_name; }
	[[nodiscard]] const std::vector<std::string>& GetPath() const { return m_path; }
	[[nodiscard]] const std::string& GetFullPath() const { return m_full_path; }
	[[nodiscard]] const std::string& GetExtension() const { return m_extension; }
	[[nodiscard]] const std::string& GetNameWithoutExtension() const { return m_name_without_extension; }
};


} // namespace alx