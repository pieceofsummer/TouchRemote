#include "StdAfx.h"
#include "FilePlaybackSource.h"
#include "Utils.h"

#pragma managed

namespace foo_touchremote
{

	FilePlaybackSource::FilePlaybackSource(const playable_location &source)
	{
		Path = FromUtf8String(source.get_path());
		Index = source.get_subsong();
	}

	bool FilePlaybackSource::Equals(IPlaybackSource^ other)
	{
		if (ReferenceEquals(other, nullptr)) return false;
		if (ReferenceEquals(other, this)) return true;

		FilePlaybackSource^ fps = safe_cast<FilePlaybackSource^>(other);
		if (fps != nullptr)
		{
			if (!String::Equals(Path, fps->Path, PATH_COMPARE_MODE)) return false;

			return Index == fps->Index;
		}

		return false;
	}

	int FilePlaybackSource::CompareTo(IPlaybackSource^ other)
	{
		if (ReferenceEquals(other, nullptr)) return -1;
		if (ReferenceEquals(other, this)) return 0;
			
		FilePlaybackSource^ fps = safe_cast<FilePlaybackSource^>(other);
		if (fps != nullptr)
		{
			int pathDiff = String::Compare(Path, fps->Path, PATH_COMPARE_MODE);
			if (pathDiff != 0) return pathDiff;

			return Index.CompareTo(fps->Index);
		}

		return 1;
	}

	String^ FilePlaybackSource::Path::get()
	{
		return m_path;
	}

	void FilePlaybackSource::Path::set(String^ value)
	{
		m_path = value;
	}

	int FilePlaybackSource::Index::get()
	{
		return m_index;
	}

	void FilePlaybackSource::Index::set(int value)
	{
		m_index = value;
	}

	String^ FilePlaybackSource::ToString()
	{
		if (String::IsNullOrEmpty(Path))
			return String::Empty;

		if (Index > 0)
			return String::Format("{0}:{1}", Path, Index);
		else
			return Path;
	}

	int FilePlaybackSource::GetHashCode()
	{
		if (String::IsNullOrEmpty(Path))
			return 0;

		return Path->GetHashCode() ^ Index;
	}

	bool FilePlaybackSource::Equals(System::Object ^other)
	{
		return Equals(safe_cast<IPlaybackSource^>(other));
	}

}