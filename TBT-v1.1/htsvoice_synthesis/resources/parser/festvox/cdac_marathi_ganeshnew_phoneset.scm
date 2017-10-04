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
;;; Phonset for cdac_marathi
;;;

;;;  Feeel free to add new feature values, or new features to this
;;;  list to make it more appropriate to your language

;; This is where it'll fall over if you haven't defined a 
;; a phoneset yet, if you have, delete this, if you haven't
;; define one then delete this error message
;;(error "You have not yet defined a phoneset for marathi (and others things ?)\n            Define it in festvox/cdac_marathi_ganeshnew_phoneset.scm\n")

(defPhoneSet
  cdac_marathi
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
   (SIL  - 0 - - - 0 0 -)  ;; slience ... 
(	a	 +	l	2	1	-	l	d	+ )
(	aa	 +	l	2	1	-	l	d	+ )
(	i	 +	l	2	1	-	l	d	+ )
(	ii	 +	l	2	1	-	l	d	+ )
(	u	 +	l	2	1	-	l	d	+ )
(	uu	 +	l	2	1	-	l	d	+ )
(	ee	 +	l	2	1	-	l	d	+ )
(	ai	 +	l	2	1	-	l	d	+ )
(	o	 +	l	2	1	-	l	d	+ )
(	au	 +	l	2	1	-	l	d	+ )
(	ao	 +	l	2	1	-	l	d	+ )
(	rq	 +	l	2	1	-	l	d	+ )
(	rrq	 +	l	2	1	-	l	d	+ )
(	ax	 +	l	2	1	-	l	d	+ )
(	ae	 +	l	2	1	-	l	d	+ )
(	k	 +	l	2	1	-	l	d	+ )
(	kh	 +	l	2	1	-	l	d	+ )
(	g	 +	l	2	1	-	l	d	+ )
(	gh	 +	l	2	1	-	l	d	+ )
(	ng	 +	l	2	1	-	l	d	+ )
(	c	 +	l	2	1	-	l	d	+ )
(	ch	 +	l	2	1	-	l	d	+ )
(	j	 +	l	2	1	-	l	d	+ )
(	jh	 +	l	2	1	-	l	d	+ )
(	nj	 +	l	2	1	-	l	d	+ )
(	tx	 +	l	2	1	-	l	d	+ )
(	txh	 +	l	2	1	-	l	d	+ )
(	dx	 +	l	2	1	-	l	d	+ )
(	dxh	 +	l	2	1	-	l	d	+ )
(	nx	 +	l	2	1	-	l	d	+ )
(	t	 +	l	2	1	-	l	d	+ )
(	th	 +	l	2	1	-	l	d	+ )
(	d	 +	l	2	1	-	l	d	+ )
(	dh	 +	l	2	1	-	l	d	+ )
(	n	 +	l	2	1	-	l	d	+ )
(	nq	 +	l	2	1	-	l	d	+ )
(	p	 +	l	2	1	-	l	d	+ )
(	ph	 +	l	2	1	-	l	d	+ )
(	b	 +	l	2	1	-	l	d	+ )
(	bh	 +	l	2	1	-	l	d	+ )
(	m	 +	l	2	1	-	l	d	+ )
(	y	 +	l	2	1	-	l	d	+ )
(	r	 +	l	2	1	-	l	d	+ )
(	rx	 +	l	2	1	-	l	d	+ )
(	l	 +	l	2	1	-	l	d	+ )
(	lx	 +	l	2	1	-	l	d	+ )
(	w	 +	l	2	1	-	l	d	+ )
(	sh	 +	l	2	1	-	l	d	+ )
(	sx	 +	l	2	1	-	l	d	+ )
(	s	 +	l	2	1	-	l	d	+ )
(	h	 +	l	2	1	-	l	d	+ )
(	kq	 +	l	2	1	-	l	d	+ )
(	khq	 +	l	2	1	-	l	d	+ )
(	gq	 +	l	2	1	-	l	d	+ )
(	z	 +	l	2	1	-	l	d	+ )
(	dxq	 +	l	2	1	-	l	d	+ )
(	dxhq	 +	l	2	1	-	l	d	+ )
(	f	 +	l	2	1	-	l	d	+ )
(	yq	 +	l	2	1	-	l	d	+ )
(	q	 +	l	2	1	-	l	d	+ )
(	mq	 +	l	2	1	-	l	d	+ )
(	hq	 +	l	2	1	-	l	d	+ )
(	ei	 +	l	2	1	-	l	d	+ )
   ;; insert the phones here, see examples in 
   ;; festival/lib/*_phones.scm

  )
)

(PhoneSet.silences '(SIL))

(define (cdac_marathi_ganeshnew::select_phoneset)
  "(cdac_marathi_ganeshnew::select_phoneset)
Set up phone set for cdac_marathi."
  (Parameter.set 'PhoneSet 'cdac_marathi)
  (PhoneSet.select 'cdac_marathi)
)

(define (cdac_marathi_ganeshnew::reset_phoneset)
  "(cdac_marathi_ganeshnew::reset_phoneset)
Reset phone set for cdac_marathi."
  t
)

(provide 'cdac_marathi_ganeshnew_phoneset)
