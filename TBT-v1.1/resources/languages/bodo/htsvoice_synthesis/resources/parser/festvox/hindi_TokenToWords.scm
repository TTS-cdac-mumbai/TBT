;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;                                                                             ;;
;;;                Rules to map tokens to words    	                        ;;
;;;                                                                             ;;
;;;  Copyright (c) 2011, vinodh <vinodh@lantana.tenet.res.in>   		;;
;;;                                                                             ;;
;;;  This program is a part of hindi voice for Festival TTS.			;;
;;;  										;;
;;;  festival hindi is free software; you can redistribute it and/or modify     ;;
;;;  it under the terms of the GNU General Public License as published by	;;
;;;  the Free Software Foundation; either version 2 of the License, or		;;
;;;  (at your option) any later version.					;;
;;;										;;
;;;  This program is distributed in the hope that it will be useful,		;;
;;;  but WITHOUT ANY WARRANTY; without even the implied warranty of		;;
;;;  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the		;;
;;;  GNU General Public License for more details.				;;
;;;										;;
;;;  You should have received a copy of the GNU General Public License		;;
;;;  along with this program; if not, write to the Free Software		;;
;;;  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA  ;;
;;;										;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(defvar hindi_dotted_abbr_list
  '(
;;Time
    ("ம" "மணி")
;;Designations
    ("டா" "டாக்டர்")
    ("சி" "சிரஞ்ஜீவி")
    ("சௌ" "சௌபாக்கியவதி")
    ("கு" "குமாரி")
    ;; Morning, Afternoon, Evening, Night
    ("மு.ப." "முற்பகல்")
    ("பி.ப." "பிற்பகல்")
;;AM and PM
    ("கா" "காலை")    
    ("மா" "மாலை") 
;;Miscellaneous
    ("நி" "நிமிடம்")
    ("௺" "நம்பர்")
    ("நெ" "நம்பர்")
    ("கி" "கிலோ")
    ("மீ" "மீட்டர்")
))

(defvar hindi_abbr_list
  '(
;;Weekdays (as defined by the POSIX te_IN locale
    ("ஞாயிறு" "ஞாயிற்றுக் கிழமை")
    ("திங்கள்" "திங்கட் கிழமை")    
    ("செவ்வாய்" "செவ்வாய்க் கிழமை")    
    ("புதன்" "புதன் கிழமை")    
    ("வியாழன்" "வியாழக் கிழமை")    
    ("வெள்ளி" "வெள்ளிக் கிழமை")    
    ("சனி" "சனிக் கிழமை")    
    ("1/2"  "அரை")
    ("1/4" "கால்")
    ("எ" "என்கிண்ற")
    ("எ.டு." "எடுத்துக்காட்டு")
    ("உ.ம்." "உதாரணம்")
))

(defvar hindi_denominations_list
  '(
   ("billionlu") ("billionla")  ("பில்லியன்")   
  ("millionlu")   ("millionla")  ("மில்லியன்")
   ("kotlu")   ("கோடியே")     ("கோடி")
    ("lakshalu")     ("லட்சத்து")     ("லட்சம்")
    ("velu")     ("ஆயிரத்து")     ("ஆயிரம்")
     ("நூற்றி")     ("நூற்று")     ("நூறு")
))

(defvar hindi_common_symbols_table
  '(
 ;   ("#" "number")
  ;  ("$" "डॉलर")
   ; ("%" "प्रतिशत")
   ; ("&" "and")
    ;("*" "star")
    ;("+" "plus")
    ;("," "comma")
    ;  ("-" "योजक")
    ("(" "ओपन" "ब्रेकेट")
 (")" "क्लोज़" "ब्रेकेट")
("/" "फ़ारवर्ड" "स्लेश")
;("\\" "बेकवर्ड" "स्लेश")
;("@" "अट")
;(":" "कोलन")
;("_" "अंडरस्कोर")
   ; ("." "dot")
   ; ("/" "forward" "slash")
  ;  (":" "colon")
    ;(";" "semi")
    ;("<" "less")
    ;("=" "equal")
    ;(">" "greater")
    ;("?" "question")
    ;("@" "at")
    ;("^" "cap")
    ;("_" ("underscore"))
    ;("`" ("tick"))
    ;("~" "tilda")
    ;("©" "copyright") ;copyright sign
    ;("®" "registered") ;registered sign
    ;("£" "pound")
    ;("€" "euro")
    ;("¥" "yen")
    ;("¢" "cent")
    ;("।" "पूर्ण" "विराम")	;Devanagari Danda
  ;  ("॥" "double" "पूर्ण" "विराम")	;Devanagari Double Danda
    ;("ॅ" "चंद्र")
   ;;ensure digits are always at the end of the table
    ;;so that they will be used as separators after other symbols have been considered
    ("0" "शून्य")
    ("1" "एक")
    ("2" "दो")
    ("3" "तीन")
    ("4" "चार")
    ("5" "पाँच")
    ("6" "छः")
    ("7" "सात")
    ("8" "आठ")
    ("9" "नौ")
))

(defvar hindi_supplementary_char_list
  '("£" "€" "¥" "©" "®" "¢" "।" "॥"))

(defvar hindi_currency_list
  '("$" "£" "€" "¥" "¢"))

(defvar hindi_abbr_markers_table
   '( ("." "") ("।" "") ("॥" "") ))
 
(defvar hindi_numbers_table
  '(
    ("௦" "0")
    ("௧" "1")
    ("௨" "2")
    ("௩" "3")
    ("௪" "4")
    ("௫" "5")
    ("௬" "6")
    ("௭" "7")
    ("௮" "8")
    ("௯" "9")
))


(define (hindi_token_to_words token name)
(flatten (hindi_token_to_words_ token name))
)

(defvar ascii_char_hash (cons-array 256));
(defvar defined_ascii_char_hash)
(define (initialize_ascii_char_hash)
(let ((char 0))
      (while (<= char 255)
        (hset ascii_char_hash (string-append (format nil "%c" char)) char)
        (set! char (+ char 1)))
(set! defined_ascii_char_hash t)
))

(define (ascii_of char)
(if (not defined_ascii_char_hash) (initialize_ascii_char_hash))
      (href ascii_char_hash char)
)

(define (hindi_match_string_start_with_list input_string string_list)
  ;;checks is any of the strings in string_list match the start of input_string
  ;;returns the matched string from string_list
  (let (substr (matched_substr ""))
  (while (and (not (null? string_list)) (string-equal matched_substr ""))
  	(set! substr (car string_list))
	(set! string_list (cdr string_list))
	(if (string-equal substr "$") 
	  	(set! match_string (string-append "\\" substr ".*"))
	  	(set! match_string (string-append substr ".*")))
  	(if (string-matches input_string match_string) (set! matched_substr substr)))
  matched_substr
)) 

(define (hindi_string_cleanup input_string)
  ;;ensures the string has characters with are either in the hindi unicode range,
  ;;ascii punctuation or represent a currency symbol
( let (curr_char next_char next_next_char (clean_string "") matched_string)
  ;;convert hindi unicode digits to ascii
  (while (and (set! matched_string (hindi_match_string_with_table input_string hindi_numbers_table))
	      (not (string-equal matched_string "")))
	(set! input_string (string-append (string-before input_string matched_string)
  	    (car (hindi_table_lookup matched_string hindi_numbers_table)) (string-after input_string matched_string))))
    (while (not (equal? input_string ""))
	(set! curr_char (ascii_of (substring input_string 0 1)))
	(set! next_char (ascii_of (substring input_string 1 1)))
	(set! next_next_char (ascii_of (substring input_string 2 1)))
	(cond
      	     ;;basic ascii
	     ((and (>= curr_char 33) (<= curr_char 126))
	          (set! num_chars_valid 1))
	     ;;hindi unicode block (of the form \340 {\244 \245} [\200 .. \277]) - In Octal representation
	     ((and (equal? curr_char 224)
		           (or (equal? next_char 164) (equal? next_char 165))
		          (and (>= next_next_char 128) (<= next_next_char 191)))
	          	  (set! num_chars_valid 3))
	     ;;Supplementary chars like yen, pound, cent, copyright, registered mark, Euro
	     ((not (string-equal (set! matched_string 
		   (hindi_match_string_start_with_list input_string hindi_supplementary_char_list)) ""))
  	           (set! num_chars_valid (string-length matched_string)))
	     ;;does not match anything, throw away the char
	     (t (set! num_chars_valid 0))
    	)
	(set! clean_string (string-append clean_string (substring input_string 0 num_chars_valid)))
	;;does not match anything, throw away the char
        (if (equal? num_chars_valid 0) (set! num_chars_valid 1))
	(set! input_string (substring input_string num_chars_valid (- (string-length input_string) num_chars_valid))))
    clean_string
))

(define (hindi_match_string_with_table input_string string_table)
  ;;checks is any of the strings in string_table match a substring of input_string
  ;;returns the matched string from string_table
  (let (substr (matched_substr ""))
  (while (and (not (null? string_table)) (string-equal matched_substr ""))
  	(set! substr (car (car string_table)))
	(set! string_table (cdr string_table))
	(if (or (string-equal substr "$") (string-equal substr "^") (string-equal substr "?")
		(string-equal substr "*") (string-equal substr "+") (string-equal substr "."))
	  	(set! match_string (string-append ".*\\" substr ".*"))
	  	(set! match_string (string-append ".*" substr ".*")))
  	(if (string-matches input_string match_string) (set! matched_substr substr)))
  matched_substr
)) 

(define (hindi_token_to_words_ token name)
  (set! name (hindi_string_cleanup name))
  (let ((number_regex "[0-9,]+\\(\\.[0-9]+\\)?") matched_substr matched_start_string prev_token_matched_start_string
						 currency_name time_segments date_segments)
  (set! matched_start_string (hindi_match_string_start_with_list name hindi_currency_list))
  (cond
    ;;currencies
   ((and (not (string-equal matched_start_string ""))
	 (string-matches (string-after name matched_start_string) number_regex))
      (set! currency_name (string-append (car (hindi_table_lookup matched_start_string hindi_common_symbols_table)) "கள்"))
      (if (hindi_list_lookup (item.feat token "n.name") hindi_denominations_list)
	  (hindi_token_to_words_ token (substring name 1  (- (string-length name) 1)))
	  (list (hindi_token_to_words_ token (substring name 1  (- (string-length name) 1))) currency_name)))
   ((and (not (string-equal (set! prev_token_matched_start_string (hindi_match_string_start_with_list 
			(item.feat token "p.name") hindi_currency_list)) ""))
       (string-matches (string-after (item.feat token "p.name") prev_token_matched_start_string) number_regex)
       (hindi_list_lookup name hindi_denominations_list)) 
           (list name (string-append 
			(car (hindi_table_lookup prev_token_matched_start_string hindi_common_symbols_table)) "கள்")))
   ;;rupees
   ((string-matches name (string-append "௹\\.[" number_regex))
        (if (hindi_list_lookup (item.feat token "n.name") hindi_denominations_list)
	  (hindi_token_to_words_ token (string-after name "௹."))
	  (list (hindi_token_to_words_ token (string-after name "௹.")) "ரூபாய்")))
    ((and (string-matches (item.feat token "p.name") (string-append "௹\\.[" number_regex))
	          (hindi_list_lookup name hindi_denominations_list) ) (list name "ரூபாய்"))
   ;;cents
   ((string-matches name (string-append number_regex "¢"))
        (list (hindi_token_to_words_ token (string-before name "¢")) "செண்டுகள்"))
    ((string-matches name "[0-9]+") (hindi_number_to_words name))
    ((string-matches name "[0-9]*\\.[0-9]+") 
        (list (hindi_number_to_words (string-before name ".")) '("புள்ளி") 
		    (mapcar hindi_number_to_words (mapcar string-append (symbolexplode (string-after name "."))))))
    ((string-matches name "\\([0-9]+,[0-9]*\\)+\\(\.[0-9]+\\)?") 
		     (hindi_token_to_words_ token (hindi_removechar name ","))) ;dd,dd,ddd.dd form
   ((string-matches name (string-append "-" number_regex)) (list '("மைனஸ்") 
							    (hindi_token_to_words_ token (string-after name "-"))))
   ((string-matches name (string-append "\\+" number_regex)) (list '("பிளஸ்") 
							    (hindi_token_to_words_ token (string-after name "+")))) 
    ;;line of characters



    ((string-matches name "_____+") (list '"அண்டர்" "ஸ்கோர்களின்" "வரிசை"))
    ((string-matches name "=====+") (list "ஈக்குவல்" "டூவின்" "வரிசை"))
    ((string-matches name "-----+") (list "ஹைஃபன்களின்" "வரிசை"))
    ((string-matches name "\\*\\*\\*\\*\\*+") (list "ஸ்டார்களின்" "வரிசை"))
    ;;time and date
    ((set! time_segments (hindi_string_matches_time name)) (mapcar hindi_token_to_words_ 
						  	  (list token token token token token token token) (mapcar string-append time_segments)))
    ((set! date_segments (hindi_string_matches_date name)) (mapcar hindi_token_to_words_ 
						  	  (list token token token) (mapcar string-append date_segments)))
    ;;abbreviations
    ;;dotted abbreviations when followed by space ex: "dr. hello"
    ((and (hindi_table_lookup name hindi_dotted_abbr_list)  (string-equal (item.feat token "punc") "."))
		(hindi_table_lookup name hindi_dotted_abbr_list))
    ;;abbreviation follwed by markers .,:,devanagari danda, double danda 
    ((and (set! matched_substr (hindi_match_string_with_table name hindi_abbr_markers_table))
	  (not (string-equal matched_substr ""))
     	  (hindi_table_lookup (string-before name matched_substr) hindi_dotted_abbr_list))
       	     (list (hindi_table_lookup (string-before name matched_substr) hindi_dotted_abbr_list)
	           (hindi_token_to_words_ token (string-after name matched_substr))))   
    ;;abbreviations not followed by .
    ((hindi_table_lookup name hindi_abbr_list) (hindi_table_lookup name hindi_abbr_list))
    ;;special characters
    ((string-matches name (string-append number_regex "%")) 
     		(list (hindi_token_to_words_ token (string-before name "%")) '("சதவிகிதம்")))
    ;;separators and connectors {#,$,%,&,*,+,-,/,<,=,>,@,\,^,_,`,copyright,registered trademark} 
    ((and (set! matched_substr (hindi_match_string_with_table name hindi_common_symbols_table) )
	  (not (string-equal matched_substr "")))
     	(list (hindi_token_to_words_ token (string-before name matched_substr))
	      (hindi_table_lookup matched_substr hindi_common_symbols_table)
	      (hindi_token_to_words_ token (string-after name matched_substr))))   
    (t 
;; (if (lts.in.alphabet (hindi_string_cleanup name) 'iitm_tam) 
	  (list (hindi_string_cleanup name)) 
;;       (list '("")))
    )
)))

;; TIME Handling
(define (hindi_string_matches_time input_string)
  ;;returns input string is in HH:MM(:SS) format
  (let ((hrs (parse-number (string-before input_string ":"))) mins secs)
  (cond 
    ((not (string-matches input_string "[0-9][0-9]?:[0-9][0-9]\\(:[0-9][0-9]\\)?")) nil)
    (t

	(set! Time "समय")
	(set! Hours "घंटे")
	(set! Minutes "मिनट")
	(set! Seconds "सेकंड")


    (set! input_string (string-after input_string ":"))
    (set! mins (string-before input_string ":"))
    (if (string-equal mins "") (set! mins (parse-number input_string)) (set! mins (parse-number mins)))
    (set! secs (string-after input_string ":"))

    ;;checking for HH,MM,SS to be in valid 24 hour format
    (if (and (>= hrs 0) (<= hrs 23) (>= mins 0) (<= mins 59) (>= (parse-number secs) 0) (<= (parse-number secs) 59))
      		(list Time hrs Hours mins Minutes secs Seconds) nil)
    )
)))

;; DATE Handling
(defvar hindi_no_of_days_in_month
  '( (1 31) (2 29)(3 31) (4 30)
     (5 31) (6 30) (7 31) (8 31)
     (9 30) (10 31) (11 30) (12 31)))

     (defvar hindi_month
	'( 
	   ("1" "january")
	   ("2" "february")
	   ("3" "march")			
	   ("4" "april")
	   ("5" "may")
	   ("6" "june")
	   ("7" "july")
	   ("8" "august")
	   ("9" "september")
	   ("10" "october")
	   ("11" "november")
	   ("12" "december"))
)



(define (hindi_string_matches_date input_string)
  ;;returns true, if the input string is in DD/MM(/YY/YYYY) format,
  (let ((date_segment1 (parse-number (string-before input_string "/"))) date_segment2 date_segment3 days_in_month)
  (cond
    ((not (string-matches input_string "[0-9][0-9]?/[0-9][0-9]?\\(/[0-9][0-9]\\([0-9][0-9]\\)?\\)?")) nil)
    (t
    (set! input_string (string-after input_string "/"))
    (set! date_segment2 (string-before input_string "/"))
    (if (string-equal date_segment2 "") (set! date_segment2 (parse-number input_string))
      					(set! date_segment2 (parse-number date_segment2)))
    (set! date_segment3 (string-after input_string "/"))
    ;;checking for DD,MM to be in valid 
    (set! days_in_month (car (hindi_table_lookup date_segment2 hindi_no_of_days_in_month)))
    (set! date_segment_month (car (hindi_table_lookup date_segment2 hindi_month)))
    (if (and days_in_month (> date_segment1 0) (<= date_segment1 days_in_month)) 
      				(list date_segment_month date_segment1 date_segment3) nil))
)))

;; NUMBER handling 
(define (hindi_number_to_words number)
  (let (input_string)
    (flatten (hindi_number_to_words_rec input_string number))
))

(define (hindi_number_to_words_rec token_list name)
  (set! name (hindi_strip_leading_zeros name)) ;remove leading zeros
  (let ((number_length (string-length name)))
  (cond
;; ;; ;; For Numbers with 8 or More Digits
    ((>= number_length 8)
          (if (string-matches name "[0-9]+0000000") ; For Numbers 1 crore 2 crores ... 10 crores ... 50 crores ... 99 crores
;; Condition TRUE 
	 (append (hindi_number_to_words_rec token_list (substring name 0 (- number_length 7))) '("करोड")) ;or kootlu
;; Condition FALSE
	     (append (hindi_number_to_words_rec token_list (substring name 0 (- number_length 7))) '("करोड") 
	           (hindi_number_to_words_rec token_list (substring name (- number_length 7) 7)))
	)
    )
;; ;; ;; For 6 and 7 Digit Numbers
    ((and (<= number_length 7) (>= number_length 6))
       (if (string-matches name "[0-9]+00000") ; For Numbers 100000 200000 ... 1000000 ... 4000000 .. 9900000
;; Condition TRUE
	 	(append token_list (hindi_two_digit_number_to_words 
				   (substring name 0 (- number_length 5))) '("लाख"));or laskalu 
;; Condition FALSE
	           (hindi_number_to_words_rec (append token_list (hindi_two_digit_number_to_words 
			(substring name 0 (- number_length 5))) '("लाख")) (substring name (- number_length 5) 5))
	)
    )
;; ;; ;; For 4 and 5 Digit Numbers
    ((and (<= number_length 5) (>= number_length 4))
       (if (string-matches name "[0-9]+000") ; For Numbers 1000, 2000 ... 10,000 ... 40,000 .. 99,000
;; 	Condition TRUE
	 	(append token_list (hindi_two_digit_number_to_words (substring name 0 (- number_length 3))) '("हजार"))
;; 	Condition FALSE
		(hindi_number_to_words_rec (append token_list (hindi_two_digit_number_to_words 
		(substring name 0 (- number_length 3))) '("हजार")) (substring name (- number_length 3) 3))
       )
    )
;; ;; ;; For 3 Digit Numbers
    ((eq number_length 3) 
      (if (string-matches name "[1-9]00") ; For 100,200,300,400 ...
  ;; Condition TRUE 
	    (append token_list (list (string-append (car (hindi_table_lookup (substring name 0 (- number_length 2)) hindi_basic_number_table)) '"सौ"))) ;or vandala
  ;; Condition FALSE  - else if
	    (if (string-matches name "[1-9][0-9][0-9]") ; For 101-199
		(hindi_number_to_words_rec  (append token_list (list (string-append  (car (hindi_table_lookup (substring name 0 (- number_length 2)) hindi_basic_number_table)) "सौ"))) (substring name (- number_length 2) 2))
	    )
       )
     )
;; ;; ;; For 2 Digit Numbers
    ((<= number_length 2) 
      (append token_list (hindi_two_digit_number_to_words name))
    )
)))

(defvar hindi_basic_number_table
  '(
    ("0" "शून्य")
    ("1" "एक")
    ("2" "दो")
    ("3" "तीन")
    ("4" "चार")
    ("5" "पाँच")
    ("6" "छः")
    ("7" "सात")
    ("8" "आठ")
    ("9" "नौ")
))

(defvar hindi_two_digit_numbers_table
  '(
 ("1" "एक")
    ("2" "दो")
    ("3" "तीन")
    ("4" "चार")
    ("5" "पाँच")
    ("6" "छः")
    ("7" "सात")
    ("8" "आठ")
    ("9" "नौ")
  ("10" "दस")
 ("11" "ग्यारह")
 ("12" "बारह")
 ("13" "तेरह")
 ("14" "चौदह")
 ("15" "पंद्रह")
 ("16" "सोलह")
 ("17" "सत्रह")
 ("18" "अठारह")
 ("19" "उन्नीस")
 ("20" "बीस")
 ("21" "इक्कीस")
 ("22" "बाईस")
 ("23" "तेईस")
 ("24" "चौबीस")
 ("25" "पच्चीस")
 ("26" "छब्बीस")
 ("28" "अठ्ठाईस")
 ("29" "उनतीस")
 ("30" "तीस")
 ("35" "पैंतीस")
 ("36" "छत्तीस")
 ("40" "चालीस")
 ("42" "बयालीस")
 ("47" "सैंतालीस")
 ("50" "पचास")
 ("70" "सत्तर")
 ("72" "बहत्तर")
 ("74" "चौहत्तर")
 ("80" "अस्सी")
 ("82" "बयासी")
 ("100" "सौ")
 ("27" "सत्ताईस")
 ("31" "इकतीस")
 ("32" "बत्तीस")
 ("33" "तैंतीस")
 ("34" "चौंतीस")
 ("36" "सैंतीस")
 ("38" "अड़तीस")
 ("39" "उनतालीस")
 ("41" "इकतालीस")
 ("43" "तैंतालीस")
 ("44" "चौवालीस")
 ("45" "पैंतालीस")
 ("46" "छियालीस")
 ("48" "अड़तालीस")
 ("49" "उनचास")
 ("51" "इक्यावन")
 ("52" "बावन")
 ("53" "तिरेपन")
 ("54" "चौवन")
 ("55" "पचपन")
 ("56" "छप्पन")
 ("57" "सत्तावन")
 ("58" "अट्ठावन")
 ("59" "उनसठ")
 ("60" "साठ")
 ("61" "इकसठ")
 ("62" "बासठ")
 ("63" "तिरेसठ")
 ("64" "चौंसठ")
 ("65" "पैंसठ")
 ("66" "छियासठ")
 ("67" "सड़सठ")
 ("68" "अड़सठ")
 ("69" "उनहत्तर")
 ("71" "इकहत्तर")
 ("73" "तिहत्तर")
 ("75" "पचहत्तर")
 ("76" "छिहत्तर")
 ("77" "सतहत्तर")
 ("78" "अठहत्तर")
 ("79" "उनासी")
 ("81" "इक्यासी")
 ("83" "तिरासी")
 ("84" "चौरासी")
 ("85" "पचासी")
 ("86" "छियासी")
 ("87" "सत्तासी")
 ("88" "अट्ठासी")
 ("89" "नवासी")
 ("90" "नब्बे")
 ("91" "इक्यानबे")
 ("92" "बानबे")
 ("93" "तिरानबे")
 ("94" "चौरानबे")
 ("95" "पंचानबे")
 ("96" "छियानबे")
 ("97" "सत्तानबे")
 ("98" "अट्ठानबे")
 ("99" "निन्यानबे")
))

;;(defvar hindi_basic_number_table
;;  '(
;;
;;   ("2" "दो")
;;    ("3" "तीन")
;;    ("4" "चार")
;;    ("5" "पाँच")
;;    ("6" "छह")
;;    ("7" "सात")
 ;;   ("8" "आठ")
;; ))

(define (hindi_list_lookup abbr lst)
 (assoc_string abbr lst))

(define (hindi_table_lookup abbr table)
  (cdr (assoc_string abbr table)))

(define (hindi_two_digit_number_to_words name)
  ;number to words for 0-99
  (let (lst units tens) 
    (set! lst (reverse (symbolexplode name)))
;; (display lst)
    (set! units (car lst))
;; (display units)
    (set! tens (car (cdr lst)))
;; (display tens)
    ;compress the units digit if it is 0
;;     (if (and (string-equal units "0") (not (or (eq tens nil) (string-equal tens "1")))) (set! units "")) 
    ;remove leading zero
    (if (string-equal tens "0") (set! tens "")) 
    (if (and (string-matches tens "[1-9]") (string-matches units "[0-9]")) 
	(hindi_table_lookup name hindi_two_digit_numbers_table)
;; TENs place is 0, and hence passing only the UNITs place for Table lookup
	(hindi_table_lookup units hindi_basic_number_table)
)))

(define (hindi_strip_leading_zeros number)
  ;removes leading zeros for a number, if single zero, leave it as it is
  (if (string-matches number "0+") (set! number "0")
       (while (string-matches number "0[0-9]*") (set! number (string-after number "0"))))
  number
)

(define (hindi_removechar input_string char)
  ;removes all occurences of char from input_string
  (let ((has_matched 1) match_string)
  (if (string-equal char "\\") (set! match_string (string-append ".*\\\\.*"))
      				 (set! match_string (string-append ".*" char ".*")))
  (while has_matched
    (if (string-matches input_string match_string)
	 (set! input_string (string-append (string-before input_string char) (string-after input_string char)))
	 (set! has_matched nil)))
    input_string))

(provide 'hindi_token)
