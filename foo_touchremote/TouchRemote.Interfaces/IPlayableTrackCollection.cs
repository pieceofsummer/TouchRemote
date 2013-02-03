using System;
using System.Collections.Generic;
using System.Text;

namespace TouchRemote.Interfaces
{
    public interface IPlayableTrackCollection
    {

        void PlayRandom();

        void Play(int index);

    }
}
