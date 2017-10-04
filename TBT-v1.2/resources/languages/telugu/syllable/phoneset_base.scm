

(define (syllable_level_lab::select_phoneset)
  "(syllable_level_lab::select_phoneset)
Set up phone set for syllable_level."
  (Parameter.set 'PhoneSet 'syllable_level)
  (PhoneSet.select 'syllable_level)
)

(define (syllable_level_lab::reset_phoneset)
  "(syllable_level_lab::reset_phoneset)
Reset phone set for syllable_level."
  t
)

(provide 'syllable_level_lab_phoneset)
