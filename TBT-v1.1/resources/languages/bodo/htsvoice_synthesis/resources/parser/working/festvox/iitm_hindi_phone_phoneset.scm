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
;;; Phonset for iitm_hindi
;;;

;;;  Feeel free to add new feature values, or new features to this
;;;  list to make it more appropriate to your language

;; This is where it'll fall over if you haven't defined a 
;; a phoneset yet, if you have, delete this, if you haven't
;; define one then delete this error message

(defPhoneSet
  iitm_hindi
  ;;;  Phone Features
  (;; vowel or consonant
   (vc + -)  
   ;; vowel length: short long dipthong schwa
   (vlng s l d a 0)
   ;; vowel height: high mid low
   (vheight 1 2 3 0 -)
   ;; vowel frontness: front mid back
   (vfront 1 2 3 0 -)
   ;; lip rounding
   (vrnd + - 0)
   ;; consonant type: stop fricative affricative nasal liquid
   (ctype s f a n l r 0)
   ;; place of articulation: labial alveolar palatal labio-dental
   ;;                         dental velar
   (cplace l a p b d v g 0)
   ;; consonant voicing
   (cvox + - 0)
   )
  (
   (SIL  -   0   0   0   0   0   0   -)  ;; slience ... 
   (a    +   s   3   3   -   0   0   0)
   (ao   +   d   2   3   +   0   0   0)
   (aa   +   l   3   3   -   0   0   0)
   (i    +   s   1   1   -   0   0   0)
   (ii   +   l   1   1   -   0   0   0)
   (u    +   s   1   3   +   0   0   0)
   (eu   +   d   1   3   +   0   0   0)
   (uu   +   l   1   3   +   0   0   0)
   (rq   -   0   0   0   0   0   a   +)
   (e    +   s   2   1   -   0   0   0)
   (ee   +   l   2   1   -   0   0   0)
   (ei   +   d   0   0   -   0   0   0)
   (ai   +   d   0   0   -   0   0   0)
   (oi   +   d   0   0   +   0   0   0)
   (o    +   s   2   3   +   0   0   0)
   (oo   +   l   2   3   +   0   0   0)
   (ae   +   d   0   0   -   0   0   0)
   (au   +   d   0   0   +   0   0   0)
   (ou   +   d   0   0   +   0   0   0)
   (k    -   0   0   0   0   s   v   -)
   (kh   -   0   0   0   0   s   v   -)
   (g    -   0   0   0   0   s   v   +)
   (gh   -   0   0   0   0   s   v   +)
   (ng   -   0   0   0   0   n   v   0)
   (c    -   0   0   0   0   a   p   -)
   (ch   -   0   0   0   0   a   p   -)
   (cx   -   0   0   0   0   a   d   -)
   (j    -   0   0   0   0   a   p   +)
   (jh   -   0   0   0   0   a   p   +)
   (jx   -   0   0   0   0   a   d   +)
   (nj   -   0   0   0   0   n   p   0)
   (tx   -   0   0   0   0   s   a   -)
   (txh  -   0   0   0   0   s   a   -)
   (dx   -   0   0   0   0   s   a   +)
   (dxh  -   0   0   0   0   s   a   +)
   (nx   -   0   0   0   0   n   a   0)
   (t    -   0   0   0   0   s   a   -)
   (th   -   0   0   0   0   s   a   -)
   (d    -   0   0   0   0   s   a   +)
   (dh   -   0   0   0   0   s   a   +)
   (n    -   0   0   0   0   n   a   0)
   (nd   -   0   0   0   0   n   a   0)
   (p    -   0   0   0   0   s   l   -)
   (ph   -   0   0   0   0   s   l   -)
   (b    -   0   0   0   0   s   l   +)
   (bh   -   0   0   0   0   s   l   +)
   (m    -   0   0   0   0   n   l   0)
   (y    -   0   0   0   0   r   p   +)
   (r    -   0   0   0   0   0   a   +)
   (l    -   0   0   0   0   l   a   +)
   (lx   -   0   0   0   0   l   a   +)
   (w    -   0   0   0   0   r   b   +)
   (sh   -   0   0   0   0   f   a   -)
   (sx   -   0   0   0   0   f   a   -)
   (s    -   0   0   0   0   f   a   -)
   (h    -   0   0   0   0   f   g   -)
   (kq   -   0   0   0   0   s   v   -)
   (khq  -   0   0   0   0   f   v   -)
   (gq   -   0   0   0   0   f   v   +)
   (z    -   0   0   0   0   f   a   +)
   (jhq  -   0   0   0   0   f   a   +)
   (dxq  -   0   0   0   0   s   a   +)
   (dxhq -   0   0   0   0   s   a   +)
   (dhq  -   0   0   0   0   s   a   +)
   (f    -   0   0   0   0   f   b   -)
   (bq   -   0   0   0   0   s   l   +)
   (yq   -   0   0   0   0   r   p   +)
   (nq   -   0   0   0   0   n   a   0)
   (rx   -   0   0   0   0   0   a   +)
   (sq   -   0   0   0   0   f   a   -)
   (zh   -   0   0   0   0   r   a   +)
   (q    -   0   0   0   0   n   a   0)
   (hq   -   0   0   0   0   f   g   -)
   (mq   -   0   0   0   0   n   a   0)
  )
)

(PhoneSet.silences '(SIL))

(define (iitm_hindi_phone::select_phoneset)
  "(iitm_hindi_phone::select_phoneset)
Set up phone set for iitm_hindi."
  (Parameter.set 'PhoneSet 'iitm_hindi)
  (PhoneSet.select 'iitm_hindi)
)

(define (iitm_hindi_phone::reset_phoneset)
  "(iitm_hindi_phone::reset_phoneset)
Reset phone set for iitm_hindi."
  t
)

(provide 'iitm_hindi_phone_phoneset)
