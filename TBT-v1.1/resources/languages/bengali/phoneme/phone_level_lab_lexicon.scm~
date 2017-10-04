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
;;; Lexicon, LTS and Postlexical rules for phone_level
;;;

;;; Load any necessary files here

(define (phone_level_addenda)
  "(phone_level_addenda)
Basic lexicon should (must ?) have basic letters, symbols and punctuation."

;;; Pronunciation of letters in the alphabet
;(lex.add.entry '("a" nn (((a) 0))))
;(lex.add.entry '("b" nn (((b e) 0))))
;(lex.add.entry '("c" nn (((th e) 0))))
;(lex.add.entry '("d" nn (((d e) 0))))
;(lex.add.entry '("e" nn (((e) 0))))
; ...

;;; Symbols ...
;(lex.add.entry 
; '("*" n (((a s) 0) ((t e) 0) ((r i1 s) 1)  ((k o) 0))))
;(lex.add.entry 
; '("%" n (((p o r) 0) ((th i e1 n) 1) ((t o) 0))))

;; Basic punctuation must be in with nil pronunciation
(lex.add.entry '("." punc nil))
;(lex.add.entry '("." nn (((p u1 n) 1) ((t o) 0))))
(lex.add.entry '("'" punc nil))
(lex.add.entry '(":" punc nil))
(lex.add.entry '(";" punc nil))
(lex.add.entry '("," punc nil))
;(lex.add.entry '("," nn (((k o1) 1) ((m a) 0))))
(lex.add.entry '("-" punc nil))
(lex.add.entry '("\"" punc nil))
(lex.add.entry '("`" punc nil))
(lex.add.entry '("?" punc nil))
(lex.add.entry '("!" punc nil))
)

(require 'lts)

;;;  Function called when word not found in lexicon
;;;  and you've trained letter to sound rules
(define (phone_level_lts_function word features)
  "(phone_level_lts_function WORD FEATURES)
Return pronunciation of word not in lexicon."

  ;; If you have nothing ...
  (format t "Unknown word %s\n" word)
  (list word features nil)

  ;; If you have lts rules (trained or otherwise)
;  (if (not boundp 'phone_level_lts_rules)
;      (require 'phone_level_lts_rules))
;  (let ((dword (downcase word)) (phones) (syls))
;    (set! phones (lts_predict dword phone_level_lts_rules))
;    (set! syls (phone_level_lex_syllabify_phstress phones))
;    (list word features syls))
  )

(define (phone_level_map_modify ps)
  (cond
   ((null ps) nil)
   ((null (cdr ps)) ps)
   ((assoc_string (string-append (car ps) (cadr ps))
                   phone_level_lab_char_phone_map)
    (cons
     (string-append (car ps) (cadr ps))
     (phone_level_map_modify (cddr ps))))
   (t
    (cons
     (car ps)
     (phone_level_map_modify (cdr ps))))))

(define (phone_level_map_phones p)
  (cond
   ((null p) nil)
   (t
    (let ((a (assoc_string (car p) phone_level_lab_char_phone_map)))
      (cond
       (a (cons (cadr a) (phone_level_map_phones (cdr p))))
       (t (phone_level_map_phones (cdr p))))))))

(define (phone_level_is_vowel x)
  (string-equal "+" (phone_feature x "vc")))

(define (phone_level_contains_vowel l)
  (member_string
   t
   (mapcar (lambda (x) (phone_level_is_vowel x)) l)))

(define (phone_level_lex_sylbreak currentsyl remainder)
  "(phone_level_lex_sylbreak currentsyl remainder)
t if this is a syl break, nil otherwise."
  (cond
   ((not (phone_level_contains_vowel remainder))
    nil)
   ((not (phone_level_contains_vowel currentsyl))
    nil)
   (t
    ;; overly naive, I mean wrong
    t))
)

(define (phone_level_lts_function word features)
"(cdac_marathi_lts_function WORD FEATURES)
Return pronunciation of word not in lexicon."
;(system "cd /home/pranaw/cdac_marathi_syllable2/")
(print "Rachana")
(print word)
(set! myfilepointer (fopen "parser.sh" "w"))
; (format myfilepointer "perl bin/il_parser_train_pranaw.pl %s " word)
;(format myfilepointer "\n perl ./bin/il_parser_train_pranaw.pl %s " word)
;(format myfilepointer "perl %s %s %s" (path-append phone_level_lab::dir "./bin/unified_parser") word phone_level_lab::dir)
(format myfilepointer "%s %s %s" (path-append phone_level_lab::dir "unified-parser") word  "0 0 0 0" )
(fclose myfilepointer)
(system "chmod +x ./parser.sh")
;(system "./parser.sh")
(system "sh ./parser.sh")
;(format t "%l\n" word)
(load "wordpronunciation")(list word nil wordstruct)
)

    ;; utf8-sampa map based on unitran 
(if (probe_file (path-append phone_level_lab::dir "festvox/phone_level_lab_char_phone_map.scm"))
    (begin
      (set! phone_level_lab_char_phone_map
            (load (path-append phone_level_lab::dir 
                               "festvox/phone_level_lab_char_phone_map.scm") t))
	(load (path-append phone_level_lab::dir 
                           "festvox/unicode_sampa_mapping.scm"))

    ;; utf8-indic-sampa letter based one
    (define (phone_level_lts_function word features)
      "(phone_level_lts_function WORD FEATURES)
Return pronunciation of word not in lexicon."
      (let ((dword word) (phones) (syls) (aphones))
        (set! aphones (phone_level_map_modify (utf8explode dword)))
        (set! phones (phone_level_map_phones aphones))
	(set! phones (sampa_lookup phones))
;        (set! phones (indic_unicode_lts sphones))
        (set! syls (phone_level_lex_syllabify_phstress phones))
        (list word features syls)))
    ))

(define (sampa_lookup gphones)
  (let ((phlist nil) (sp nil))
    (mapcar 
     (lambda (gg)
       (set! sp (assoc_string gg unicode_sampa_mapping))
       (if sp
           (set! phlist (append (car (cadr sp)) phlist))
           (set! phlist (cons gg phlist))))
     gphones)
    (reverse phlist)))

(define (indic_unicode_lts phlist)
	(set! finallist (list))
	(set! graphemecount 0)
	(set! prevgrapheme (list))
	(set! totgcnt (- (length phlist) 1))
	(mapcar (lambda (ggg)
		(if (symbol? (car ggg))
		(begin
		(cond
			;; schwa deletion for the last consonant
			((equal? graphemecount totgcnt)
			(begin
				(if (string-equal (phone_feature (car ggg) 'vc) "-")
				(begin 
					(if (string-equal (phone_feature (car prevgrapheme) 'vc) "-") 
					(set! finallist (append  finallist prevgrapheme)))
					;(set! finallist (append finallist (list (car ggg)))) ;appropriate for hindi
					(set! finallist (append finallist  ggg)) ; for generic (non-schwa final) indic
				)
				(begin 
					(if (string-equal (phone_feature (car prevgrapheme) 'vc) "-") 
					(set! finallist (append finallist (list (car prevgrapheme)))))
					(set! finallist (append finallist (list (car ggg))))
				))
			))
			;; generic treatment for an intermediate grapheme
			((and (> graphemecount 0) (< graphemecount totgcnt))
			(begin
				(cond 
					;; If current is vowel, remove the previous schwa
					((and (string-equal (phone_feature (car ggg) 'vc) "+") (string-equal (phone_feature (car prevgrapheme) 'vc) "-"))
					(begin 
						(set! finallist (append finallist (list (car prevgrapheme))))
						(set! finallist (append finallist (list (car ggg))))
					))
					;; If current is consonant and previous is consonant, dump all of previous 
					((and  (string-equal (phone_feature (car ggg) 'vc) "-") (string-equal (phone_feature (car prevgrapheme) 'vc) "-"))
					(set! finallist (append finallist prevgrapheme)))
					(t 
					 t)
				)
			))
			((and (eq? graphemecount 0) (string-equal (phone_feature (car ggg) 'vc) "+"))
				(set! finallist (list (car ggg)))
			)
			(t 
			t)
		)
		(set! graphemecount (+ 1 graphemecount))
		(set! prevgrapheme ggg)
		)
		(begin 
			(cond
				((equal? (car ggg) '(P))
					(set! finallist (append finallist (list (car prevgrapheme))))
					(set! prevgrapheme (list))
				)
				((equal? (car ggg) '(M))
					(if (string-equal (phone_feature (car prevgrapheme) 'vc) "-") (set! finallist (append finallist prevgrapheme)))
					(set! finallist (append finallist (list "nB")))
					(set! prevgrapheme (list))
				)
				((equal? (car ggg) '(CD))
					(if (string-equal (phone_feature (car prevgrapheme) 'vc) "-") (set! finallist (append finallist prevgrapheme)))
					(set! finallist (append finallist (list "nB")))
					(set! prevgrapheme (list))
				)
				(t
				t)
				;(format t "debug: todo \n")
			)
			(set! graphemecount (+ 1 graphemecount))
		)
	)
	) phlist)
finallist)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;; OR: Hand written letter to sound rules
;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; ;;;  Function called when word not found in lexicon
; (define (phone_level_lts_function word features)
;   "(phone_level_lts_function WORD FEATURES)
; Return pronunciation of word not in lexicon."
;   (format stderr "failed to find pronunciation for %s\n" word)
;   (let ((dword (downcase word)))
;     ;; Note you may need to use a letter to sound rule set to do
;     ;; casing if the language has non-ascii characters in it.
;     (if (lts.in.alphabet word 'phone_level)
; 	(list
; 	 word
; 	 features
; 	 ;; This syllabification is almost certainly wrong for
; 	 ;; this language (its not even very good for English)
; 	 ;; but it will give you something to start off with
; 	 (lex.syllabify.phstress
; 	   (lts.apply word 'phone_level)))
; 	(begin
; 	  (format stderr "unpronouncable word %s\n" word)
; 	  ;; Put in a word that means "unknown" with its pronunciation
; 	  '("nepoznat" nil (((N EH P) 0) ((AO Z) 0) ((N AA T) 0))))))
; )

; ;; You may or may not be able to write a letter to sound rule set for
; ;; your language.  If its largely lexicon based learning a rule
; ;; set will be better and easier that writing one (probably).
; (lts.ruleset
;  phone_level
;  (  (Vowel WHATEVER) )
;  (
;   ;; LTS rules 
;   ))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;; Postlexical Rules 
;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define (phone_level::postlex_rule1 utt)
  "(phone_level::postlex_rule1 utt)
A postlexical rule form correcting phenomena over word boundaries."
  (mapcar
   (lambda (s)
     ;; do something
     )
   (utt.relation.items utt 'Segment))
   utt)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;; Lexicon definition
;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(lex.create "phone_level")
(lex.set.phoneset "phone_level")
(lex.set.lts.method 'phone_level_lts_function)
(if (probe_file (path-append phone_level_lab::dir "festvox/phone_level_lex.out"))
    (lex.set.compile.file (path-append phone_level_lab::dir 
                                       "festvox/phone_level_lex.out")))
(phone_level_addenda)
(if (probe_file (path-append phone_level_lab::dir "festvox/phone_level_addenda.scm"))
    (load (path-append phone_level_lab::dir "festvox/phone_level_addenda.scm")))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;; Lexicon setup
;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define (phone_level_lab::select_lexicon)
  "(phone_level_lab::select_lexicon)
Set up the lexicon for phone_level."
  (lex.select "phone_level")

  ;; Post lexical rules
  (set! postlex_rules_hooks (list phone_level::postlex_rule1))
)

(define (phone_level_lab::reset_lexicon)
  "(phone_level_lab::reset_lexicon)
Reset lexicon information."
  t
)

(provide 'phone_level_lab_lexicon)
