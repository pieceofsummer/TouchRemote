#pragma once

using namespace System;
using namespace System::Collections;
using namespace System::Diagnostics;
using namespace TouchRemote::Interfaces;

namespace foo_touchremote 
{

	public ref class FilePlaybackSource : public IPlaybackSource
	{
	public:
		FilePlaybackSource(const playable_location &source);
		
		virtual bool Equals(IPlaybackSource ^other);

		virtual int CompareTo(IPlaybackSource ^other);

		property String^ Path
		{
			void set(String^ value);
			String^ get();
		};

		property int Index
		{
			void set(int value);
			int get();
		};

		virtual String^ ToString() override;
		virtual bool Equals(Object^ other) override;
		virtual int GetHashCode() override;

	private:
		String^ m_path;
		int m_index;
	};
}
