\version "2.18.2"
\language "deutsch"

\header {
  % Voreingestellte LilyPond-Tagline entfernen
  tagline = ##f
}

\paper {
  #(set-paper-size "a4")
}

global = {
  \key c \major
  \time 4/4
}

scoreAViolinI = \relative c'' {
  \global
  g1
  
}

scoreAViolinII = \relative c' {
  \global
  c1
  
}

scoreAViolinIPart = \new Staff \with {
  instrumentName = "Vl. I"
  midiInstrument = "violin"
} \scoreAViolinI

scoreAViolinIIPart = \new Staff \with {
  instrumentName = "Vl. II"
  midiInstrument = "violin"
} \scoreAViolinII

\score {
  <<
    \scoreAViolinIPart
    \scoreAViolinIIPart
  >>
  \layout { }
  \midi {
    \tempo 4=100
  }
}
