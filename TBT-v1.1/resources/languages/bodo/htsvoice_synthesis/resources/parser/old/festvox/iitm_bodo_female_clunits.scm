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
;;;  Customized for: iitm_bodo_female                                       ;;
;;;                                                                      ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;; Ensure this version of festival has been compiled with clunits module
(require_module 'clunits)
(require 'clunits) ;; runtime scheme support

;;; Try to find the directory where the voice is, this may be from
;;; .../festival/lib/voices/ or from the current directory
(if (assoc 'iitm_bodo_female_clunits voice-locations)
    (defvar iitm_bodo_female::dir 
      (cdr (assoc 'iitm_bodo_female_clunits voice-locations)))
    (defvar iitm_bodo_female::dir (string-append (pwd) "/")))

;;; Did we succeed in finding it
(if (not (probe_file (path-append iitm_bodo_female::dir "festvox/")))
    (begin
     (format stderr "iitm_bodo_female::clunits: Can't find voice scm files they are not in\n")
     (format stderr "   %s\n" (path-append  iitm_bodo_female::dir "festvox/"))
     (format stderr "   Either the voice isn't linked in Festival library\n")
     (format stderr "   or you are starting festival in the wrong directory\n")
     (error)))

;;;  Add the directory contains general voice stuff to load-path
(set! load-path (cons (path-append iitm_bodo_female::dir "festvox/") 
		      load-path))

;;; Voice specific parameter are defined in each of the following
;;; files
(require 'iitm_bodo_female_phoneset)
(require 'iitm_bodo_female_tokenizer)
(require 'iitm_bodo_female_tagger)
(require 'iitm_bodo_female_lexicon)
(require 'iitm_bodo_female_phrasing)
(require 'iitm_bodo_female_intonation)
(require 'iitm_bodo_female_duration)
(require 'iitm_bodo_female_f0model)
(require 'iitm_bodo_female_other)
;; ... and others as required

;;;
;;;  Code specific to the clunits waveform synthesis method
;;;

;;; Flag to save multiple loading of db
(defvar iitm_bodo_female::clunits_loaded nil)
;;; When set to non-nil clunits voices *always* use their closest voice
;;; this is used when generating the prompts
(defvar iitm_bodo_female::clunits_prompting_stage nil)
;;; Flag to allow new lexical items to be added only once
(defvar iitm_bodo_female::clunits_added_extra_lex_items nil)

;;; You may wish to change this (only used in building the voice)
(set! iitm_bodo_female::closest_voice 'voice_kal_diphone_bodo)

(set! bodo_phone_maps
      '(
;        (M_t t)
;        (M_dH d)
        ))

(define (voice_kal_diphone_bodo_phone_maps utt)
  (mapcar
   (lambda (s) 
     (let ((m (assoc_string (item.name s) bodo_phone_maps)))
       (if m
           (item.set_feat s "us_diphone" (cadr m))
           (item.set_feat s "us_diphone"))))
   (utt.relation.items utt 'Segment))
  utt)

(define (voice_kal_diphone_bodo)
  (voice_kal_diphone)
  (set! UniSyn_module_hooks (list voice_kal_diphone_bodo_phone_maps ))

  'kal_diphone_bodo
)

;;;  These are the parameters which are needed at run time
;;;  build time parameters are added to his list in iitm_bodo_female_build.scm
(set! iitm_bodo_female::dt_params
      (list
       (list 'db_dir iitm_bodo_female::dir)
       '(name iitm_bodo_female)
       '(index_name iitm_bodo_female)
       '(f0_join_weight 0.0)
       '(join_weights
         (0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 ))
       '(trees_dir "festival/trees/")
       '(catalogue_dir "festival/clunits/")
       '(coeffs_dir "mcep/")
       '(coeffs_ext ".mcep")
       '(clunit_name_feat lisp_iitm_bodo_female::clunit_name)
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

(define (iitm_bodo_female::nextvoicing i)
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

(define (iitm_bodo_female::clunit_name i)
  "(iitm_bodo_female::clunit_name i)
Defines the unit name for unit selection for bodo.  The can be modified
changes the basic classification of unit for the clustering.  By default
this we just use the phone name, but you may want to make this, phone
plus previous phone (or something else)."
  (let ((name (item.name i)))
    (cond
     ((and (not iitm_bodo_female::clunits_loaded)
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
       (iitm_bodo_female::nextvoicing i)))
     ((string-equal name "pau")
      (string-append
       name
       "_"
       (iitm_bodo_female::nextvoicing i)))
     (t
      (string-append
       name
       "_"
;       (item.feat i "seg_onsetcoda")
;       "_"
       (iitm_bodo_female::nextvoicing i))))))

(define (iitm_bodo_female::clunits_load)
  "(iitm_bodo_female::clunits_load)
Function that actual loads in the databases and selection trees.
SHould only be called once per session."
  (set! dt_params iitm_bodo_female::dt_params)
  (set! clunits_params iitm_bodo_female::dt_params)
  (clunits:load_db clunits_params)
  (load (string-append
	 (string-append 
	  iitm_bodo_female::dir "/"
	  (get_param 'trees_dir dt_params "trees/")
	  (get_param 'index_name dt_params "all")
	  ".tree")))
  (set! iitm_bodo_female::clunits_clunit_selection_trees clunits_selection_trees)
  (set! iitm_bodo_female::clunits_loaded t))

(define (iitm_bodo_female::voice_reset)
  "(iitm_bodo_female::voice_reset)
Reset global variables back to previous voice."
  (iitm_bodo_female::reset_phoneset)
  (iitm_bodo_female::reset_tokenizer)
  (iitm_bodo_female::reset_tagger)
  (iitm_bodo_female::reset_lexicon)
  (iitm_bodo_female::reset_phrasing)
  (iitm_bodo_female::reset_intonation)
  (iitm_bodo_female::reset_duration)
  (iitm_bodo_female::reset_f0model)
  (iitm_bodo_female::reset_other)

  t
)

;; This function is called to setup a voice.  It will typically
;; simply call functions that are defined in other files in this directory
;; Sometime these simply set up standard Festival modules othertimes
;; these will be specific to this voice.
;; Feel free to add to this list if your language requires it

(define (voice_iitm_bodo_female_clunits)
  "(voice_iitm_bodo_female_clunits)
Define voice for bodo."
  ;; *always* required
  (voice_reset)

  ;; Select appropriate phone set
  (iitm_bodo_female::select_phoneset)

  ;; Select appropriate tokenization
  (iitm_bodo_female::select_tokenizer)

  ;; For part of speech tagging
  (iitm_bodo_female::select_tagger)

  (iitm_bodo_female::select_lexicon)
  ;; For clunits selection you probably don't want vowel reduction
  ;; the unit selection will do that
  (if (string-equal "americanenglish" (Param.get 'Language))
      (set! postlex_vowel_reduce_cart_tree nil))

  (iitm_bodo_female::select_phrasing)

  (iitm_bodo_female::select_intonation)

  (iitm_bodo_female::select_duration)

  (iitm_bodo_female::select_f0model)

  ;; Waveform synthesis model: clunits

  ;; Load in the clunits databases (or select it if its already loaded)
  (if (not iitm_bodo_female::clunits_prompting_stage)
      (begin
	(if (not iitm_bodo_female::clunits_loaded)
	    (iitm_bodo_female::clunits_load)
	    (clunits:select 'iitm_bodo_female))
	(set! clunits_selection_trees 
	      iitm_bodo_female::clunits_clunit_selection_trees)
	(Parameter.set 'Synth_Method 'Cluster)))

  ;; This is where you can modify power (and sampling rate) if desired
  (set! after_synth_hooks nil)
;  (set! after_synth_hooks
;      (list
;        (lambda (utt)
;          (utt.wave.rescale utt 2.1))))

  (set! current_voice_reset iitm_bodo_female::voice_reset)

  (set! current-voice 'iitm_bodo_female_clunits)
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

(provide 'iitm_bodo_female_clunits)

