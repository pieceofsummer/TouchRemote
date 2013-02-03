#pragma once

using namespace System;
using namespace TouchRemote::Interfaces;

namespace foo_touchremote
{

	public ref class Album : public IAlbum
	{
	public:
		Album(IMediaLibrary^ library, IArtist^ artist, String^ title);

		virtual bool Equals(IAlbum ^other);

		virtual int CompareTo(IAlbum ^other);

		virtual property int Id
		{
			int get();
		}

		virtual property __int64 PersistentId
		{
			__int64 get();
		}

		virtual property IArtist^ Artist
		{
			IArtist^ get();
		}

		virtual property String^ Title
		{
			String^ get();
		}

		virtual String^ ToString() override;
		virtual bool Equals(Object^ other) override;
		virtual int GetHashCode() override;

	private:
		IMediaLibrary^ m_library;
		IArtist^ m_artist;
		String^ m_title;

	};
}