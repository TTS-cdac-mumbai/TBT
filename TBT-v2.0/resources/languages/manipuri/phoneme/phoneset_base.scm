

(define (phone_level_lab::select_phoneset)
  "(phone_level_lab::select_phoneset)
Set up phone set for phone_level."
  (Parameter.set 'PhoneSet 'phone_level)
  (PhoneSet.select 'phone_level)
)

(define (phone_level_lab::reset_phoneset)
  "(phone_level_lab::reset_phoneset)
Reset phone set for phone_level."
  t
)

(provide 'phone_level_lab_phoneset)
