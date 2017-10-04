;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;                                                                     ;;;
;;;                     Carnegie Mellon University                      ;;;
;;;                  and Alan W Black and Kevin Lenzo                   ;;;
;;;                      Copyright (c) 1998-2005                        ;;;
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
;;;                                                                      ;;
;;;  A generic voice definition file for a clunits synthesizer           ;;
;;;  Customized for: phone_level_lab                                       ;;
;;;                                                                      ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;; Ensure this version of festival has been compiled with clunits module
(require_module 'clunits)
(require 'clunits) ;; runtime scheme support

;;; Try to find the directory where the voice is, this may be from
;;; .../festival/lib/voices/ or from the current directory
(if (assoc 'phone_level_lab_clunits voice-locations)
    (defvar phone_level_lab::dir 
      (cdr (assoc 'phone_level_lab_clunits voice-locations)))
    (defvar phone_level_lab::dir (string-append (pwd) "/")))

;;; Did we succeed in finding it
(if (not (probe_file (path-append phone_level_lab::dir "festvox/")))
    (begin
     (format stderr "phone_level_lab::clunits: Can't find voice scm files they are not in\n")
     (format stderr "   %s\n" (path-append  phone_level_lab::dir "festvox/"))
     (format stderr "   Either the voice isn't linked in Festival library\n")
     (format stderr "   or you are starting festival in the wrong directory\n")
     (error)))

;;;  Add the directory contains general voice stuff to load-path
(set! load-path (cons (path-append phone_level_lab::dir "festvox/") 
		      load-path))

;;; Voice specific parameter are defined in each of the following
;;; files
(require 'phone_level_lab_phoneset)
(require 'phone_level_lab_tokenizer)
(require 'phone_level_lab_tagger)
(require 'phone_level_lab_lexicon)
(require 'phone_level_lab_phrasing)
(require 'phone_level_lab_intonation)
(require 'phone_level_lab_duration)
(require 'phone_level_lab_f0model)
(require 'phone_level_lab_other)
;; ... and others as required

;;;
;;;  Code specific to the clunits waveform synthesis method
;;;

;;; Flag to save multiple loading of db
(defvar phone_level_lab::clunits_loaded nil)
;;; When set to non-nil clunits voices *always* use their closest voice
;;; this is used when generating the prompts
(defvar phone_level_lab::clunits_prompting_stage nil)
;;; Flag to allow new lexical items to be added only once
(defvar phone_level_lab::clunits_added_extra_lex_items nil)

;;; You may wish to change this (only used in building the voice)
(set! phone_level_lab::closest_voice 'voice_kal_diphone_level)

(set! level_phone_maps
      '(
;        (M_t t)
;        (M_dH d)
        ))

(define (voice_kal_diphone_level_phone_maps utt)
  (mapcar
   (lambda (s) 
     (let ((m (assoc_string (item.name s) level_phone_maps)))
       (if m
           (item.set_feat s "us_diphone" (cadr m))
           (item.set_feat s "us_diphone"))))
   (utt.relation.items utt 'Segment))
  utt)

(define (voice_kal_diphone_level)
  (voice_kal_diphone)
  (set! UniSyn_module_hooks (list voice_kal_diphone_level_phone_maps ))

  'kal_diphone_level
)

;;;  These are the parameters which are needed at run time
;;;  build time parameters are added to his list in phone_level_lab_build.scm
(set! phone_level_lab::dt_params
      (list
       (list 'db_dir phone_level_lab::dir)
       '(name phone_level_lab)
       '(index_name phone_level_lab)
       '(f0_join_weight 0.0)
       '(join_weights
         (0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 ))
       '(trees_dir "festival/trees/")
       '(catalogue_dir "festival/clunits/")
       '(coeffs_dir "mcep/")
       '(coeffs_ext ".mcep")
       '(clunit_name_feat lisp_phone_level_lab::clunit_name)
       ;;  Run time parameters 
       '(join_method windowed)
       ;; if pitch mark extraction is bad this is better than the above
;       '(join_method smoothedjoin)
;       '(join_method modified_lpc)
       '(continuity_weight 5)
;       '(log_scores 1)  ;; good for high variance joins (not so good for ldom)
       '(optimal_coupling 1)
       '(extend_selections 2)
       '(pm_coeffs_dir "mcep/")
       '(pm_coeffs_ext ".mcep")
       '(sig_dir "wav/")
       '(sig_ext ".wav")
;       '(pm_coeffs_dir "lpc/")
;       '(pm_coeffs_ext ".lpc")
;       '(sig_dir "lpc/")
;       '(sig_ext ".res")
;       '(clunits_debug 1)
))

(define (phone_level_lab::nextvoicing i)
  (let ((nname (item.feat i "n.name")))
    (cond
;     ((string-equal nname "pau")
;      "PAU")
     ((string-equal "+" (item.feat i "n.ph_vc"))
      "V")
     ((string-equal (item.feat i "n.ph_cvox") "+")
      "CVox")
     (t
      "UV"))))

(define (phone_level_lab::clunit_name i)
  "(phone_level_lab::clunit_name i)
Defines the unit name for unit selection for level.  The can be modified
changes the basic classification of unit for the clustering.  By default
this we just use the phone name, but you may want to make this, phone
plus previous phone (or something else)."
  (let ((name (item.name i)))
    (cond
     ((and (not phone_level_lab::clunits_loaded)
	   (or (string-equal "h#" name) 
	       (string-equal "1" (item.feat i "ignore"))
	       (and (string-equal "pau" name)
		    (or (string-equal "pau" (item.feat i "p.name"))
			(string-equal "h#" (item.feat i "p.name")))
		    (string-equal "pau" (item.feat i "n.name")))))
      "ignore")
     ;; Comment out this if you want a more interesting unit name
     ((null nil)
      name)

     ;; Comment out the above if you want to use these rules
     ((string-equal "+" (item.feat i "ph_vc"))
      (string-append
       name
       "_"
       (item.feat i "R:SylStructure.parent.stress")
       "_"
       (phone_level_lab::nextvoicing i)))
     ((string-equal name "pau")
      (string-append
       name
       "_"
       (phone_level_lab::nextvoicing i)))
     (t
      (string-append
       name
       "_"
;       (item.feat i "seg_onsetcoda")
;       "_"
       (phone_level_lab::nextvoicing i))))))

(define (phone_level_lab::clunits_load)
  "(phone_level_lab::clunits_load)
Function that actual loads in the databases and selection trees.
SHould only be called once per session."
  (set! dt_params phone_level_lab::dt_params)
  (set! clunits_params phone_level_lab::dt_params)
  (clunits:load_db clunits_params)
  (load (string-append
	 (string-append 
	  phone_level_lab::dir "/"
	  (get_param 'trees_dir dt_params "trees/")
	  (get_param 'index_name dt_params "all")
	  ".tree")))
  (set! phone_level_lab::clunits_clunit_selection_trees clunits_selection_trees)
  (set! phone_level_lab::clunits_loaded t))

(define (phone_level_lab::voice_reset)
  "(phone_level_lab::voice_reset)
Reset global variables back to previous voice."
  (phone_level_lab::reset_phoneset)
  (phone_level_lab::reset_tokenizer)
  (phone_level_lab::reset_tagger)
  (phone_level_lab::reset_lexicon)
  (phone_level_lab::reset_phrasing)
  (phone_level_lab::reset_intonation)
  (phone_level_lab::reset_duration)
  (phone_level_lab::reset_f0model)
  (phone_level_lab::reset_other)

  t
)

;; This function is called to setup a voice.  It will typically
;; simply call functions that are defined in other files in this directory
;; Sometime these simply set up standard Festival modules othertimes
;; these will be specific to this voice.
;; Feel free to add to this list if your language requires it

(define (voice_phone_level_lab_clunits)
  "(voice_phone_level_lab_clunits)
Define voice for level."
  ;; *always* required
  (voice_reset)

  ;; Select appropriate phone set
  (phone_level_lab::select_phoneset)

  ;; Select appropriate tokenization
  (phone_level_lab::select_tokenizer)

  ;; For part of speech tagging
  (phone_level_lab::select_tagger)

  (phone_level_lab::select_lexicon)
  ;; For clunits selection you probably don't want vowel reduction
  ;; the unit selection will do that
  (if (string-equal "americanenglish" (Param.get 'Language))
      (set! postlex_vowel_reduce_cart_tree nil))

  (phone_level_lab::select_phrasing)

  (phone_level_lab::select_intonation)

  (phone_level_lab::select_duration)

  (phone_level_lab::select_f0model)

  ;; Waveform synthesis model: clunits

  ;; Load in the clunits databases (or select it if its already loaded)
  (if (not phone_level_lab::clunits_prompting_stage)
      (begin
	(if (not phone_level_lab::clunits_loaded)
	    (phone_level_lab::clunits_load)
	    (clunits:select 'phone_level_lab))
	(set! clunits_selection_trees 
	      phone_level_lab::clunits_clunit_selection_trees)
	(Parameter.set 'Synth_Method 'Cluster)))

  ;; This is where you can modify power (and sampling rate) if desired
  (set! after_synth_hooks nil)
;  (set! after_synth_hooks
;      (list
;        (lambda (utt)
;          (utt.wave.rescale utt 2.1))))

  (set! current_voice_reset phone_level_lab::voice_reset)

  (set! current-voice 'phone_level_lab_clunits)
)

(define (is_pau i)
  (if (phone_is_silence (item.name i))
      "1"
      "0"))

(define (cg_break s)
  "(cg_break s)
0, if word internal, 1 if word final, 4 if phrase final, we ignore 
3/4 distinguinction in old syl_break"
  (let ((x (item.feat s "syl_break")))
    (cond
     ((string-equal "0" x)
      (string-append x)
      )
     ((string-equal "1" x)
      (string-append x)
      )
     ((string-equal "0" (item.feat s "R:SylStructure.parent.n.name"))
      "4")
     (t
      "3"))))

(provide 'phone_level_lab_clunits)

