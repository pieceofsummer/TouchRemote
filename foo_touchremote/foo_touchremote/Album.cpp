#include "stdafx.h"
#include "Album.h"
#include "Library.h"
#include "IDProvider.h"

#pragma managed

namespace foo_touchremote
{

	Album::Album(IMediaLibrary ^library, IArtist ^artist, String ^title)
	{
		if (library == nullptr)
			throw gcnew ArgumentNullException("library");
		if (artist == nullptr)
			throw gcnew ArgumentNullException("artist");
		if (String::IsNullOrEmpty(title))
			throw gcnew ArgumentNullException("title");

		m_library = library;
		m_artist = artist;
		m_title = title;
	}

	IArtist^ Album::Artist::get()
	{
		return m_artist;
	}

	String^ Album::Title::get()
	{
		return m_title;
	}

	bool Album::Equals(IAlbum^ other)
	{
		if (other == nullptr) return false;

		if (!((IEquatable<IArtist^>^)Artist)->Equals(other->Artist)) return false;

		if (!String::Equals(Title, other->Title, StringComparison::InvariantCultureIgnoreCase)) return false;

		return true;
	}

	int Album::CompareTo(IAlbum^ other)
	{
		if (other == nullptr) return -1;
			
		int diff = Artist->CompareTo(other->Artist);
		if (diff != 0) return diff;

		return String::Compare(Title, other->Title, StringComparison::InvariantCultureIgnoreCase);
	}

	String^ Album::ToString()
	{
		return String::Format("{0} - {1}", Artist, Title);
	}

	int Album::GetHashCode()
	{
		return Artist->GetHashCode() ^ Title->GetHashCode();
	}

	bool Album::Equals(System::Object ^other)
	{
		return Equals(safe_cast<IAlbum^>(other));
	}

	int Album::Id::get()
	{
		return ((Library^)m_library)->Identifiers->GetId(this);
	}

	__int64 Album::PersistentId::get()
	{
		return ((Library^)m_library)->Identifiers->GetPersistentId(this);
	}
}