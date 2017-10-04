;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;                                                                     ;;;
;;;                     Carnegie Mellon University                      ;;;
;;;                  and Alan W Black and Kevin Lenzo                   ;;;
;;;                      Copyright (c) 1998-2000                        ;;;
;;;                        All Rights Reserved.                         ;;;
;;;                                                                     ;;;
;;; Permission is hereby granted, free of charge, to use and distribute ;;;
;;; this software and its documentation without restriction, including  ;;;
;;; without limitation the rights to use, copy, modify, merge, publish, ;;;
;;; distribute, sublicense, and/or sell copies of this work, and to     ;;;
;;; permit persons to whom this work is furnished to do so, subject to  ;;;
;;; the following conditions:                                           ;;;
;;;  1. The code must retain the above copyright notice, this list of   ;;;
;;;     conditions and the following disclaimer.                        ;;;
;;;  2. Any modifications must be clearly marked as such.               ;;;
;;;  3. Original authors' names are not deleted.                        ;;;
;;;  4. The authors' names are not used to endorse or promote products  ;;;
;;;     derived from this software without specific prior written       ;;;
;;;     permission.                                                     ;;;
;;;                                                                     ;;;
;;; CARNEGIE MELLON UNIVERSITY AND THE CONTRIBUTORS TO THIS WORK        ;;;
;;; DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING     ;;;
;;; ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT  ;;;
;;; SHALL CARNEGIE MELLON UNIVERSITY NOR THE CONTRIBUTORS BE LIABLE     ;;;
;;; FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES   ;;;
;;; WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN  ;;;
;;; AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,         ;;;
;;; ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF      ;;;
;;; THIS SOFTWARE.                                                      ;;;
;;;                                                                     ;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;; Tokenizer for marathi
;;;
;;;  To share this among voices you need to promote this file to
;;;  to say festival/lib/cdac_marathi/ so others can use it.
;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;; Load any other required files

;; Punctuation for the particular language
(set! cdac_marathi_ganeshnew::token.punctuation "\"'`.,:;!?(){}[]")
(set! cdac_marathi_ganeshnew::token.prepunctuation "\"'`({[")
(set! cdac_marathi_ganeshnew::token.whitespace " \t\n\r")
;;(set! cdac_marathi_ganeshnew::token.singlecharsymbols "")
(set! cdac_marathi_ganeshnew::token.singlecharsymbols "\" '")

;;; Voice/marathi token_to_word rules 
(define (cdac_marathi_ganeshnew::token_to_words token name)
  "(cdac_marathi_ganeshnew::token_to_words token name)
Specific token to word rules for the voice cdac_marathi_ganeshnew.  Returns a list
of words that expand given token with name."
  (cond
   ((string-matches name "[1-9][0-9]+")
    (cdac_marathi::number token name))
   (t ;; when no specific rules apply do the general ones
    (list name))))

(define (cdac_marathi::number token name)
  "(cdac_marathi::number token name)
Return list of words that pronounce this number in marathi."

  (error "cdac_marathi::number to be written\n")

)

(define (cdac_marathi_ganeshnew::select_tokenizer)
  "(cdac_marathi_ganeshnew::select_tokenizer)
Set up tokenizer for marathi."
  (Parameter.set 'Language 'cdac_marathi)
  (set! token.punctuation cdac_marathi_ganeshnew::token.punctuation)
  (set! token.prepunctuation cdac_marathi_ganeshnew::token.prepunctuation)
  (set! token.whitespace cdac_marathi_ganeshnew::token.whitespace)
  (set! token.singlecharsymbols cdac_marathi_ganeshnew::token.singlecharsymbols)

  (set! token_to_words cdac_marathi_ganeshnew::token_to_words)
)

(define (cdac_marathi_ganeshnew::reset_tokenizer)
  "(cdac_marathi_ganeshnew::reset_tokenizer)
Reset any globals modified for this voice.  Called by 
(cdac_marathi_ganeshnew::voice_reset)."
  ;; None

  t
)

(provide 'cdac_marathi_ganeshnew_tokenizer)
