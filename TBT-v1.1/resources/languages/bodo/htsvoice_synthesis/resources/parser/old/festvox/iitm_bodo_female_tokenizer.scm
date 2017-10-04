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
;;; Tokenizer for bodo
;;;
;;;  To share this among voices you need to promote this file to
;;;  to say festival/lib/iitm_bodo/ so others can use it.
;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;; Load any other required files

;; Punctuation for the particular language
(set! iitm_bodo_female::token.punctuation "\"'`.,:;!?(){}[]")
(set! iitm_bodo_female::token.prepunctuation "\"'`({[")
(set! iitm_bodo_female::token.whitespace " \t\n\r")
(set! iitm_bodo_female::token.singlecharsymbols "-")

;;; Voice/bodo token_to_word rules 
(define (iitm_bodo_female::token_to_words token name)
  "(iitm_bodo_female::token_to_words token name)
Specific token to word rules for the voice iitm_bodo_female.  Returns a list
of words that expand given token with name."
  (cond
   ((string-matches name "[1-9][0-9]+")
    (iitm_bodo::number token name))
   (t ;; when no specific rules apply do the general ones
    (list name))))

(define (iitm_bodo::number token name)
  "(iitm_bodo::number token name)
Return list of words that pronounce this number in bodo."

  (error "iitm_bodo::number to be written\n")

)

(define (iitm_bodo_female::select_tokenizer)
  "(iitm_bodo_female::select_tokenizer)
Set up tokenizer for bodo."
  (Parameter.set 'Language 'iitm_bodo)
  (set! token.punctuation iitm_bodo_female::token.punctuation)
  (set! token.prepunctuation iitm_bodo_female::token.prepunctuation)
  (set! token.whitespace iitm_bodo_female::token.whitespace)
  (set! token.singlecharsymbols iitm_bodo_female::token.singlecharsymbols)

  (set! token_to_words iitm_bodo_female::token_to_words)
)

(define (iitm_bodo_female::reset_tokenizer)
  "(iitm_bodo_female::reset_tokenizer)
Reset any globals modified for this voice.  Called by 
(iitm_bodo_female::voice_reset)."
  ;; None

  t
)

(provide 'iitm_bodo_female_tokenizer)
