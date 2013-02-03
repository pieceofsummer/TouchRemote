#include "stdafx.h"
#include "Artist.h"
#include "Library.h"
#include "IDProvider.h"

#pragma managed

namespace foo_touchremote
{

	Artist::Artist(IMediaLibrary^ library, String^ name)
	{
		if (library == nullptr)
			throw gcnew ArgumentNullException("library");
		if (String::IsNullOrEmpty(name))
			throw gcnew ArgumentNullException("name");

		m_library = library;
		m_name = name;
	}

	String^ Artist::Name::get()
	{
		return m_name;
	}

	bool Artist::Equals(IArtist^ other)
	{
		if (other == nullptr) return false;

		return String::Equals(Name, other->Name, StringComparison::InvariantCultureIgnoreCase);
	}

	int Artist::CompareTo(IArtist^ other)
	{
		if (other == nullptr) return -1;

		return String::Compare(Name, other->Name, StringComparison::InvariantCultureIgnoreCase);
	}

	String^ Artist::ToString()
	{
		return Name;
	}

	int Artist::GetHashCode()
	{
		return Name->GetHashCode();
	}

	bool Artist::Equals(System::Object ^other)
	{
		return Equals(safe_cast<IArtist^>(other));
	}

	int Artist::Id::get()
	{
		return ((Library^)m_library)->Identifiers->GetId(this);
	}

	__int64 Artist::PersistentId::get()
	{
		return ((Library^)m_library)->Identifiers->GetPersistentId(this);
	}
}