;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;                                                                             ;;
;;;                Rules to map tokens to words    	                        ;;
;;;                                                                             ;;
;;;  Copyright (c) 2006, Priti Patil, janabhaaratii, C-DAC, Mumbai              ;;
;;;                      <prithisd@cdacmumbai.in>, <prithisd@gmail.com>		;;
;;;                                                                             ;;
;;;  Copyright (c) 2005, Chaitanya Kamisetty <chaitanya@atc.tcs.co.in> 		;;
;;;                                                                             ;;
;;;  This program is a part of festival-mr.					;;
;;;  										;;
;;;  festival-mr is free software; you can redistribute it and/or modify        ;;
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

;; Abbreviations when followed by a dot and a space ex: "Dr. Hello"
;; or when followed by one of the symbols listed in marathi_abbr_markers_table
;; are replaced with their full forms
(defvar marathi_dotted_abbr_list
  '(
    ;; Time
    ("ता." "तास") ;; taasa - Hours
    ("मि." "मिनीटं") ;; minITa.n - Minutes
    ;; Designations
    ("डॉ" "डॉक्टर") ;; DA.ckTara - Doctor 
    ("चि" "चिरंजीव") ;; chira.njIva - A title used for men 
    ("सौ" "सौभाग्यवती") ;; saubhAgyavatI - A title used for women 
    ("कु" "कुमारी") ;; kumArI - A title used for bachlorettes
    ;; Morning, Afternoon, Evening, Night - There are four zones instead of just AM PM
    ("प" "पहाटे") ;; pahATe - Early morning
    ("स" "सकाळी") ;; sakALI - Morning
    ("दु" "दुपारी") ;; dupArI - Afternoon
    ("सं" "संध्याकाळी") ;; sa.ndhyAkALI - Evening
    ("रा" "रात्री") ;; rAtrI - Night
    ;; Rough equivalents of AM and PM from Sanskrit
    ("पू" "पूर्वान्ह:") ;; poorvaahna
    ("अ" "अपरान्ह:") ;; apraahna
))

;; Above list contains items that will be followed by a dot or other character
;; Here the list contains abbreviations that are not followed by any puncuation
;; but need to be expanded anyway
(defvar marathi_abbr_list
  '(
    ;; Weekdays (as defined by the POSIX mr_IN locale
    ("रवि" "रविवार") ;; ravivAra - Sunday
    ("सोम" "सोमवार") ;; somavAra - Monday
    ("मंगळ" "मंगळवार") ;; ma.ngaLavAra - Tuesday
    ("बुध" "बुधवार") ;; budhavAra - Wednesday
    ("गुरू" "गुरूवार") ;; gurUvAra - Thirsday
    ("शुक्र" "शुक्रवार") ;; shukravAra - Friday
    ("शनि" "शनिवार") ;; shanivAra - Saturday
    ("1/2"  "अर्ध") ;; ardha - Half
    ("1/4" "पाव") ;; pAva - Quarter
   ; ("ता" "तास") ;; taasa - Hours
))

;; If these words follow a currency, then we need to 
;; change the order of pronounciation for this word,
;; number and the currency symbol
(defvar marathi_denominations_list
  '(
   ("बिलिअन्स") ("बिलिअन")   
   ("मिलिअन्स") ("मिलिअन")
   ("करोड") ("कोटी")
   ("लाख") ("लक्ष")
   ("हजार") ("हजारो")
  ("शे") ("शेकडा") ("शंभर")
))

;; This list the symbols that need to be spoken out
;; If a symbol is found in marathi_currency_list it should have
;; an expansion in this list. Other than currency list, all other 
;; symbols are the last to be processed in the tokenising process
(defvar marathi_common_symbols_table
  '(
    ("#" "हॅश") ;; ha.csha - Hash
    ("$" "डॉलर") ;; DA.clara - Dollor
    ("%" "टक्के") ;; Takke - Percentage
    ("&" "आणि") ;; ANi - And
    ("*" "गुणिले") ;; guNile - Multiply
    ("+" "अधिक") ;; adhika - Plus
   ;;    ("," "स्वल्पविराम") ;; svalpavirAma - Comma
    ("," "") ;; svalpavirAma - Comma
    
   ;; ("." "") ;; svalpavirAma - Comma
;;    ("-" "वजा") ;; vajA - Minus
    ("-" "योजक") ;; vajA - Minus
   ("." ".") ;; retain the dot
   ("।" "।") ;; retain the danda simbol
 ; ("?" "?");; retain the question mark
;;    ("/" "भागिले") ;; bhAgile - By
    ("/" "फ़ारवर्ड" "स्लेश") ;; bhAgile - By
	("(" "ओपन" "ब्रेकेट")
    (")" "क्लोज़" "ब्रेकेट")
;;    (":" "कोलन") ;; kolan - Colon
    (":" "कोलन") ;; kolan - Colon
    (";" "सेमिकोलन") ;; eat up the semi colon
    ("<" ("पेक्षा" "लहान")) ;; pexA lahAna dyaan - Less Than
    ("=" "बरोबर") ;; barobara - Equal to
    (">" ("पेक्षा" "मोठा")) ;; pexA moThA - Greater than
    ("?" "क्वेश्चन" "मार्क") ;; eat up the question mark
    ("@" "अॅट") ;; a.cTa - At
    ("^" "कैप") ;; eat up Caret
    ("_" ("अंडर" "स्कोर")) ;; a.nDara skora - Under Score
    ("`" ("बॅक" "कोट")) ;; ba.cka koTa - Back Quote
    ("~" "टिल्डा") ;; TilDA - Tilde
    ("©" "कॉपीराईट") ;; kA.cpIrAiTa - Copyright Sign
    ("®" "रजिस्टर्ड") ;; rajisTarDa - Registered Sign
    ("£" "पाऊंड") ;; pAU.nDa - Pound
    ("€" "युरो") ;; yuro - Euro
    ("¥" "येन") ;; yena - Yen
    ("¢" "सेन्ट") ;; senTa - Cent
    ("।" "")	;; Eat up Devanagari Danda
    ("॥" "")	;; Eat up Devanagari Double Danda
    ;;ensure digits are always at the end of the table
    ;;so that they will be used as separators after other symbols have been considered
    ("0" "शून्य") ;; shUnya - Zero
    ("1" "एक") ;; eka - One
    ("2" "दोन") ;; dona - Two
    ("3" "तीन") ;; tIna - Three
    ("4" "चार") ;; chAra - Four
    ("5" "पाच") ;; pAcha - Five
    ("6" "सहा") ;; sahA - Six
    ("7" "सात") ;; sAta - Seven
    ("8" "आठ") ;; ATha - Eight
    ("9" "नऊ") ;; naU - Nine
))

;; These are the additional characters allowed by the string
;; cleanup function
(defvar marathi_supplementary_char_list
  '("£" "€" "¥" "©" "®" "¢" "।" "॥"))

;; The text is checked if it contains these items almost at the
;; begining of the processing and full forms will be substitued 
;; by taking the full form from common symbols table
(defvar marathi_currency_list
  '( "$" "£" "€" "¥"))

;; If the items in dotted_abbr_list is followed by these symbols
;; Then they are conisered a match and expanded otherwise
;; they are not a match. Dot will be parsed by festival specially
;; so there is also a special handling for it in the code
(defvar marathi_abbr_markers_table
   '( ("." "") ("।" "") ("॥" "") ))
 
;; Marathi numbers are converted to westren numerals during the
;; string cleanup
(defvar marathi_numbers_table
  '(
    ("०" "0") ;; Zero
    ("१" "1") ;; One
    ("२" "2") ;; Two
    ("३" "3") ;; Three
    ("४" "4") ;; Four
    ("५" "5") ;; Five
    ("६" "6") ;; Six
    ("७" "7") ;; Seven
    ("८" "8") ;; Eight
    ("९" "9") ;; Nine
))

;; This is main function of this file. This gets called
;; from outside. the function is expected to return a string
;; of words that is the normalised form of the string
;; The output string is later sent to letter to sound rules
(define (hindi_token_to_words token name)
    (flatten (marathi_token_to_words_ token name))
)

;; A way to convert a given char to its ascii value
;; Could not find any better/direct way to do it
(defvar ascii_char_hash (cons-array 256));
(defvar defined_ascii_char_hash)
(define (initialize_ascii_char_hash)
(let ((char 0))
      (while (<= char 255)
        (hset ascii_char_hash (string-append (format nil "%c" char)) char)
        (set! char (+ char 1)))
(set! defined_ascii_char_hash t)
))

;; Convert a char into ascii value using above initialised table
(define (ascii_of char)
(if (not defined_ascii_char_hash) (initialize_ascii_char_hash))
      (href ascii_char_hash char)
)

;; A utility function for looking up text in the tables
(define (marathi_match_string_start_with_list input_string string_list)
  ;; Checks if any of the strings in string_list match the start of input_string
  ;; Returns the matched string from string_list
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

;; Cleans up the string to discard any charater that we are not going to handle
;; What we are going to handle is ascii, Devanagari (Marathi) unicode block, special symbols like euro etc.
(define (marathi_string_cleanup input_string)
  ;; Ensures the string has characters with are either in the marathi unicode range,
  ;; ascii punctuation or represent a currency symbol
(let (curr_char next_char next_next_char (clean_string "") matched_string)
  ;; Convert Marathi unicode digits to ascii
  (while (and (set! matched_string (marathi_match_string_with_table input_string marathi_numbers_table))
	      (not (string-equal matched_string "")))
	(set! input_string (string-append (string-before input_string matched_string)
  	    (car (marathi_table_lookup matched_string marathi_numbers_table)) (string-after input_string matched_string))))
    ;; Iterate over all the characters in the string
    ;; We also need 2 look ahead characters for dirty handling for UTF-8 text
    (while (not (equal? input_string ""))
	(set! curr_char (ascii_of (substring input_string 0 1)))
	(set! next_char (ascii_of (substring input_string 1 1)))
	(set! next_next_char (ascii_of (substring input_string 2 1)))
	(cond
      	     ;; Basic ascii
	     ((or (or (and (>= curr_char 33) (<= curr_char 64)) (and (>= curr_char 91) (<= curr_char 96)))
		  (and (>= curr_char 123) (<= curr_char 126)))
	          (set! num_chars_valid 1))
             ;; FIXME: bring this part out so that code be separate from language specific knowledge
	     ;; Marathi unicode block (of the form \340 {\244 \245} [\200 .. \277]) 
	     ((and (equal? curr_char 224)
		           (or (equal? next_char 164) (equal? next_char 165))
		          (and (>= next_next_char 128) (<= next_next_char 191)))
	          	  (set! num_chars_valid 3))
	     ;; Supplementary chars like yen, pound, cent, copyright, registered mark, Euro
	     ((not (string-equal (set! matched_string 
		   (marathi_match_string_start_with_list input_string marathi_supplementary_char_list)) ""))
  	           (set! num_chars_valid (string-length matched_string)))
	     ;; Does not match anything, throw away the char
	     (t (set! num_chars_valid 0))
    	)
	(set! clean_string (string-append clean_string (substring input_string 0 num_chars_valid)))
	;; Does not match anything, throw away the char
        (if (equal? num_chars_valid 0) (set! num_chars_valid 1))
	(set! input_string (substring input_string num_chars_valid (- (string-length input_string) num_chars_valid))))
    clean_string
))

;; A utility function for looking up text in the tables
(define (marathi_match_string_with_table input_string string_table)
  ;; Checks is any of the strings in string_table match a substring of input_string
  ;; Returns the matched string from string_table
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

;; Most of the work happens here. From here we call all the other functions in the file
;; Called by marathi_tokne_to_words
(define (marathi_token_to_words_ token name)
 (print "name")  
 (print name)
  (set! name (marathi_string_cleanup name))

  ;; Numbers are to be matched a lot place and they contain commas and decimals
  (let ((number_regex "[0-9,]+\\(\\.[0-9]+\\)?") matched_substr matched_start_string prev_token_matched_start_string
						 currency_name time_segments date_segments)

  (set! matched_start_string (marathi_match_string_start_with_list name marathi_currency_list))
  (cond

    ;; FIXME: rewrite this so that code be separate from language specific knowledge
    ;; FIXME: the message translation can be generalised using $1 $2 etc. inside the strings

    ;; Currencies
    ;; When currencies are followed by billions etc., we need to change the order
    ;; Rs. 1000 Billion will become 1000 billion rupees
    ((and (not (string-equal matched_start_string ""))
	 (string-matches (string-after name matched_start_string) number_regex))
      (set! currency_name (string-append (car (marathi_table_lookup matched_start_string marathi_common_symbols_table)) ""))
      (if (marathi_list_lookup (item.feat token "n.name") marathi_denominations_list)
	  (marathi_token_to_words_ token (substring name 1  (- (string-length name) 1)))
	  (list (marathi_token_to_words_ token (substring name 1  (- (string-length name) 1))) currency_name)))
    ((and (not (string-equal (set! prev_token_matched_start_string (marathi_match_string_start_with_list 
			(item.feat token "p.name") marathi_currency_list)) ""))
       (string-matches (string-after (item.feat token "p.name") prev_token_matched_start_string) number_regex)
       (marathi_list_lookup name marathi_denominations_list)) 
           (list name (string-append 
			(car (marathi_table_lookup prev_token_matched_start_string marathi_common_symbols_table)) "")))

    ;; Rupees
    ((string-matches name (string-append "रू\\.[" number_regex))
        (if (marathi_list_lookup (item.feat token "n.name") marathi_denominations_list)
	  (marathi_token_to_words_ token (string-after name "रू."))
	  (list (marathi_token_to_words_ token (string-after name "रू.")) "रूपए")))
    ((and (string-matches (item.feat token "p.name") (string-append "रू\\.[" number_regex))
	          (marathi_list_lookup name marathi_denominations_list) ) (list name "रूपए"))

    ;; Cents - the symbol occurs after the number
    ((string-matches name (string-append number_regex "¢"))
        (list (marathi_token_to_words_ token (string-before name "¢")) "सेंट्स")) ;; se.nTsa - Cents

    ;; Simple numbers
    ((string-matches name "[0-9]+") (marathi_number_to_words name))
    ;; Numbers with decimal point
    ((string-matches name "[0-9]*\\.[0-9]+") 
        (list (marathi_number_to_words (string-before name ".")) '("पुर्णांक") ;; purNA.nka - Point
		    (mapcar marathi_number_to_words (mapcar string-append (symbolexplode (string-after name "."))))))
    ;; Numbers of the form dd,dd,ddd.dd
    ((string-matches name "\\([0-9]+,[0-9]*\\)+\\(\.[0-9]+\\)?") 
		     (marathi_token_to_words_ token (marathi_removechar name ",")))
    ;; Numbers with a minus in the front
    ((string-matches name (string-append "-" number_regex)) (list '("उणे") ;; uNe - Minus
							    (marathi_token_to_words_ token (string-after name "-"))))
    ;; Numbers with a plus in the front
    ((string-matches name (string-append "\\+" number_regex)) (list '("अधिक") ;; adhika - Plus
							    (marathi_token_to_words_ token (string-after name "+")))) 

    ;; Line of characters
    ((string-matches name "_____+") (list '"अंडर" "स्कोरची" "रेघ")) ;; a.nDara skorachI regha - A line of under scores
    ((string-matches name "=====+") (list "इक्वलटू" "ची" "रेघ")) ;; ikvalTUchI regha , FIXME: unnecessary splits
    ((string-matches name "-----+") (list "हायफन्सची" "रेघ")) ;; hAyaphansachI regha - A line of hyphens, FIXME: unnecessary splits
    ((string-matches name "\\*\\*\\*\\*\\*+") (list "अॅस्टरिस्क्सची" "रेघ")) ;;  : a line of asterisks

    ;; Time and Date
    ((set! time_segments (marathi_string_matches_time name)) (mapcar marathi_token_to_words_ 
						  	  (list token token token) (mapcar string-append time_segments)))
    ((set! date_segments (marathi_string_matches_date name)) (mapcar marathi_token_to_words_ 
						  	  (list token token token) (mapcar string-append date_segments)))

    ;; Abbreviations
    ;; Dotted abbreviations when followed by space ex: "Dr. Hello"
    ((and (marathi_table_lookup name marathi_dotted_abbr_list)  (string-equal (item.feat token "punc") "."))
		(marathi_table_lookup name marathi_dotted_abbr_list))
;(print "name1")
;(print name)
    ;; Abbreviation follwed by markers .,:,devanagari danda, double danda 
    ((and (set! matched_substr (marathi_match_string_with_table name marathi_abbr_markers_table))
	  (not (string-equal matched_substr ""))
     	  (marathi_table_lookup (string-before name matched_substr) marathi_dotted_abbr_list))
       	     (list (marathi_table_lookup (string-before name matched_substr) marathi_dotted_abbr_list)
	           (marathi_token_to_words_ token (string-after name matched_substr))))   

    ;; Abbreviations not followed by .
    ((marathi_table_lookup name marathi_abbr_list) (marathi_table_lookup name marathi_abbr_list))

    ;; Number followed by percentages
    ((string-matches name (string-append number_regex "%")) 
     		(list (marathi_token_to_words_ token (string-before name "%")) '("टक्के"))) ;; Takke - Percentage
    ;; Separators and connectors {#,$,%,&,*,+,-,/,<,=,>,@,\,^,_,`,copyright,registered trademark} 
    ((and (set! matched_substr (marathi_match_string_with_table name marathi_common_symbols_table) )
	  (not (string-equal matched_substr "")))
     	(list (marathi_token_to_words_ token (string-before name matched_substr))
	      (marathi_table_lookup matched_substr marathi_common_symbols_table)
	      (marathi_token_to_words_ token (string-after name matched_substr))))   

    ;; Nothing else todo, return
  ;;  (t (if (lts.in.alphabet (marathi_string_cleanup name) 'hindi) (list (marathi_string_cleanup name)) (list '(""))))

   (t 
;; (if (lts.in.alphabet (hindi_string_cleanup name) 'iitm_tam) 
	  (list (marathi_string_cleanup name)) 
;;       (list '("")))
    )


)))

;; Time matching function
(define (marathi_string_matches_time input_string)
  ;; Returns input string is in HH:MM(:SS) format
  (let ((hrs (parse-number (string-before input_string ":"))) mins secs)
  (cond 
    ((not (string-matches input_string "[0-9][0-9]?:[0-9][0-9]\\(:[0-9][0-9]\\)?")) nil)
    (t
    (set! input_string (string-after input_string ":"))
    (set! mins (string-before input_string ":"))
    (if (string-equal mins "") (set! mins (parse-number input_string)) (set! mins (parse-number mins)))
    (set! secs (string-after input_string ":"))

    ;; Checking for HH,MM,SS to be in valid 24 hour format
    (if (and (>= hrs 0) (<= hrs 23) (>= mins 0) (<= mins 59) (>= (parse-number secs) 0) (<= (parse-number secs) 59))
      		(list hrs mins secs) nil)
    )
)))

;; Storing the number of days in a month
;; so we can check for a valid date
(defvar marathi_no_of_days_in_month
  '( (1 31) (2 29)(3 31) (4 30)
     (5 31) (6 30) (7 31) (8 31)
     (9 30) (10 31) (11 30) (12 31)))

;; Date matching function
(define (marathi_string_matches_date input_string)
  ;; Returns true is the input string is in DD/MM(/YY/YYYY) format,
  (let ((date_segment1 (parse-number (string-before input_string "/"))) date_segment2 date_segment3 days_in_month)
  (cond
    ((not (string-matches input_string "[0-9][0-9]?/[0-9][0-9]?\\(/[0-9][0-9]\\([0-9][0-9]\\)?\\)?")) nil)
    (t
    (set! input_string (string-after input_string "/"))
    (set! date_segment2 (string-before input_string "/"))
    (if (string-equal date_segment2 "") (set! date_segment2 (parse-number input_string))
      					(set! date_segment2 (parse-number date_segment2)))
    (set! date_segment3 (string-after input_string "/"))
    ;; Checking for DD,MM to be in valid 
    (set! days_in_month (car (marathi_table_lookup date_segment2 marathi_no_of_days_in_month)))
    (if (and days_in_month (> date_segment1 0) (<= date_segment1 days_in_month)) 
      				(list date_segment1 date_segment2 date_segment3) nil))
)))

;; Convert numbers in the string to words in the language
;; Inturn calls another function
(define (marathi_number_to_words number)
  (let (input_string)
    (flatten (marathi_number_to_words_rec input_string number))
))

;; Convert numbers in the string to words in the language
(define (marathi_number_to_words_rec token_list name)
  ;; FIXME: rewrite so that code be separate from language specific knowledge
  (set! name (marathi_strip_leading_zeros name)) ;; Remove leading zeros
  (let ((number_length (string-length name)))
  (cond
    ;; If number is in crores we handle it here
    ((>= number_length 8)
     ;; Special case for 1 crore
     (if (string-equal name "10000000") (append token_list '("कोटी")) ;; koTI - Crore
       ;; Case for round figured crores like 2 crores and 23 crores
       (if (string-matches name "[0-9]+0000000")
	 (append (marathi_number_to_words_rec token_list (substring name 0 (- number_length 7))) '("कोटी")) ;; koTI - crore 
         ;;Case for one crore plus change
	 (if (string-matches name "1[0-9][0-9][0-9][0-9][0-9][0-9][0-9]") 
	     (append  token_list '("एक" "कोटी") (marathi_number_to_words_rec token_list (substring name (- number_length 7) 7))) ;; eka koTI - One crore 12987600, 10099887, etc
             ;; Default case if the number is other than above
	     (append (marathi_number_to_words_rec token_list (substring name 0 (- number_length 7))) '("कोटी") ;; koTI
	           (marathi_number_to_words_rec token_list (substring name (- number_length 7) 7))))))) 
    ;; If number is in lakhs we handle it here
    ((and (<= number_length 7) (>= number_length 6))
     ;; Special case for 1 lakh
     (if (string-equal name "100000") (append token_list '("लक्ष")) ;; laksha- Lakh
       ;; Case for round figured lakhs like 2 lakhs and 56 lakhs
       (if (string-matches name "[0-9]+00000")
	 	(append token_list (marathi_two_digit_number_to_words (substring name 0 (- number_length 5))) '("लक्ष")) ;; laksha - lakhs 200000, 500000, etc
                ;;Case for one lakh plus change
		(if (string-matches name "1[0-9][0-9][0-9][0-9][0-9]")
        	   (marathi_number_to_words_rec (append token_list '("एक" "लक्ष")) (substring name (- number_length 5) 5)) ;; ek laksha - One laksha 198734, 178900, etc
                   ;; Default case if the number is other than above
	           (marathi_number_to_words_rec (append token_list (marathi_two_digit_number_to_words
			(substring name 0 (- number_length 5))) '("लक्ष")) (substring name (- number_length 5) 5)))))) ;; laksh
    ;; If number is in thousands we handle it here
    ((and (<= number_length 5) (>= number_length 4)) 
     ;; Special case for 1 thousand
     (if (string-equal name "1000") (append token_list '("हजार")) ;; hajAra - Thousand
       ;; Case for round figured thousands like 2 thousand and 56 thousand
       (if (string-matches name "[0-9]+000")
	 	(append token_list (marathi_two_digit_number_to_words (substring name 0 (- number_length 3))) '("हजार")) ;; hajAra - Thousands 2000, 7000, 3000, etc
         ;; Case for 4 digit numbers like 1100, 1200, 2300 
     	 (if (string-matches name "[0-9][1-9]00")
	   	(append token_list (marathi_two_digit_number_to_words 
				     (substring name 0 (- number_length 2))) '("शे")) ;; she - hundreds 2800, 1100, 8900, etc
                ;; Case for one thousand plus change
		(if (string-matches name "1[0-9][0-9][0-9]")
                   (marathi_number_to_words_rec (append (marathi_two_digit_number_to_words (substring name 0 2))
						token_list '("शे"))  (substring name 2 2)) ;; she - hundreds 1234, 5673, 9009, etc
                   ;; Default case if the number is other than above
	           (marathi_number_to_words_rec (append token_list (marathi_two_digit_number_to_words 
			(substring name 0 (- number_length 3))) '("हजार")) (substring name (- number_length 3) 3))))))) ;; hajAra - thousand
    ;; If number is less than thousand, we handle it here
    ((eq number_length 3)
     ;; Special case for one hundred
     (if (string-equal name "100") (append token_list '("शंभर"))	;; sha.nbhara - Hundred rupees
        ;; Case for round figured hundreds like 2 hundred and 9 hundred
        (if (string-matches name "[2-9]00")
	   (append token_list (marathi_two_digit_number_to_words 
				(substring name 0 (- number_length 2))) '("शे")) ;; she -  hundred 200, 300, 400, etc
           ;; Case for one hundred plus change
           (if (string-matches name "1[0-9][0-9]") (append token_list '("एक" "शे") ;; she - Hundred and.. 111, 109, 178, etc
					        	(marathi_two_digit_number_to_words (substring name 1 2)))
            ;; Default case if the number is other than above
	    (marathi_number_to_words_rec (append token_list (marathi_two_digit_number_to_words 
			(substring name 0 (- number_length 2))) '("शे")) (substring name (- number_length 2) 2)))))) ;; she - hundred 987, 290, etc
    ;; If the number is less than hundred we call as function to handle it
    ((<= number_length 2) (append token_list (marathi_two_digit_number_to_words name)))
    ((= number_length 1) (append token_list (marathi_number_to_words name)))
)))

;; This table is queried by the two digit number to word converter
;; for coverting the units place
(defvar marathi_basic_number_table
  '(
    ("0" "शून्य") ;;shUnya - Zero
    ("1" "एक") ;;eka - One
    ("2" "दोन") ;;dona - Two
    ("3" "तीन") ;;tIna - Three
    ("4" "चार") ;;chaara - Four
    ("5" "पाच") ;;pAcha - Five
    ("6" "सहा") ;;sahA - Six
    ("7" "सात") ;;sAta - Seven
    ("8" "आठ") ;;ATha - Eight
    ("9" "नऊ") ;;naU - Nine
))

;; This table is queried by the two digit number to word converter
;; for coverting the tens place and special numbers from 11 to 19
(defvar marathi_two_digit_numbers_table
  '(
    ("10" "दहा") ;; dahA - Ten
    ("11" "अकरा") ;; akarA - Eleven
    ("12" "बारा") ;; bArA - Twelve
    ("13" "तेरा") ;; terA - Thirteen
    ("14" "चौदा") ;; chaudA - Fourteen
    ("15" "पंधरा") ;; pa.ndharA - Fifteen
    ("16" "सोळा") ;; soLA - Sixteen
    ("17" "सतरा") ;; satarA - Seventeen
    ("18" "अठरा") ;; aTharA - Eighteen
    ("19" "एकोणीस") ;; ekoNIsa - Nineteen
    ("20" "वीस") ;; vIsa - Twenty

    ("21" "एकवीस")
    ("22" "बावीस")
    ("23" "तेवीस")
    ("24" "चोवीस")
    ("25" "पंचवीस")
    ("26" "सव्वीस")
    ("27" "सत्तावीस")
    ("28" "अठ्ठावीस")
    ("29" "एकोणतीस")
    ("30" "तीस") ;; tIsa - Thirty

    ("31" "एकतीस")
    ("32" "बत्तीस")
    ("33" "तेहत्तीस")
    ("34" "चौतीस")
    ("35" "पस्तीस")
    ("36" "छत्तीस")
    ("37" "सदतीस")
    ("38" "अडतीस")
    ("39" "एकोणचाळीस")
    ("40" "चाळीस") ;; chALIsa - Fourty

    ("41" "एक्केचाळीस")
    ("42" "बेचाळीस")
    ("43" "त्रेचाळीस")
    ("44" "चव्वेचाळीस")
    ("45" "पंचेचाळीस")
    ("46" "शेहचाळीस")
    ("47" "सत्तेचाळीस")
    ("48" "अठ्ठेचाळीस")
    ("49" "एकोणपन्नास")
    ("50" "पन्नास") ;; pannAsa - Fifty

    ("51" "एक्कावन्न")
    ("52" "बावन्न")
    ("53" "त्रेपन्न")
    ("54" "चोपन्न")
    ("55" "पंचावन्न")
    ("56" "छप्पन्न")
    ("57" "सत्तावन्न")
    ("58" "अठ्ठावन्न")
    ("59" "एकोणसाठ")
    ("60" "साठ") ;; sATha - Sixty

    ("61" "एकसष्ठ")
    ("62" "बासष्ठ")
    ("63" "त्रेसष्ठ")
    ("64" "चौसष्ठ")
    ("65" "पासष्ठ")
    ("66" "सहासष्ठ")
    ("67" "सदुसष्ठ")
    ("68" "अडुसष्ठ")
    ("69" "एकोणसत्तर")
    ("70" "सत्तर") ;; sattara - Seventy

    ("71" "एक्कात्तर")
    ("72" "बहात्तर")
    ("73" "त्रेहात्तर")
    ("74" "चौर्यात्तर")
    ("75" "पंच्यात्तर")
    ("76" "शहात्तर")
    ("77" "सत्त्यात्तर")
    ("78" "अठ्ठयात्तर")
    ("79" "एकोणैंशी")
    ("80" "ऐंशी") ;; ai.nshI - Eighty

    ("81" "एक्क्यैंशी")
    ("82" "ब्यैंशी")
    ("83" "त्र्यैंशी")
    ("84" "चौर्यैंशी")
    ("85" "पंच्यैंशी")
    ("86" "शह्यैंशी")
    ("87" "सत्त्यैंशी")
    ("88" "अठ्यैंशी")
    ("89" "एकोणनव्वद")
    ("90" "नव्वद") ;; navvada - Ninety

    ("91" "एक्क्याण्णव")
    ("92" "बयाण्णव")
    ("93" "त्रयाण्णव")
    ("94" "चौर्याण्णव")
    ("95" "पंचयाण्णव")
    ("96" "शहाण्णव")
    ("97" "सतयाण्णव")
    ("98" "अठयाण्णव")
    ("99" "नवयाण्णव")
))

;; Utility function for list lookup
(define (marathi_list_lookup abbr lst)
 (assoc_string abbr lst))

;; Utility function for table lookup
(define (marathi_table_lookup abbr table)
  (cdr (assoc_string abbr table)))

;; Convert two digit numbers to words
;; Used by number to words function
;; Takes non zero numers
(define (marathi_two_digit_number_to_words name)
  ;; Number to words for 0-99
  ;; FIXME: is this language dependent?
  (let (lst units tens) 
    (set! lst (reverse (symbolexplode name)))
    (set! units (car lst))
    (set! tens (car (cdr lst)))
    ;; Supress the units digit if it is 0
    (if (and (string-equal units "0") (not (or (eq tens nil) (string-equal tens "1")))) (set! units "")) 
    ;; Remove leading zero
    (if (string-equal tens "0") (set! tens "")) 
    (if (string-matches tens "[1-9]") (marathi_table_lookup name marathi_two_digit_numbers_table)
	(cons (marathi_table_lookup tens marathi_two_digit_numbers_table) (marathi_table_lookup units marathi_basic_number_table))

 	;; above line is valid till "2 digit numbers to words" are english alike viz. 21-twenty-one, 95-ninety-five etc, hence not applicable to all languages like in marathi 21-ekavIsa (२१-एकवीस) etc.
;;	(marathi_table_lookup name marathi_two_digit_numbers_table)
;;	(marathi_table_lookup name marathi_basic_number_table)
)))

;; Used by number for words before any processing to remove
;; the leading zeros
(define (marathi_strip_leading_zeros number)
  ;; Removes leading zeros for a number, if single zero, leave it as it is
  (if (string-matches number "0+") (set! number "0")
       (while (string-matches number "0[0-9]*") (set! number (string-after number "0"))))
  number
)

;; Utility function to remove a particular charater from a string
(define (marathi_removechar input_string char)
  ;; Removes all occurences of char from input_string
  (let ((has_matched 1) match_string)
  (if (string-equal char "\\") (set! match_string (string-append ".*\\\\.*"))
      				 (set! match_string (string-append ".*" char ".*")))
  (while has_matched
    (if (string-matches input_string match_string)
	 (set! input_string (string-append (string-before input_string char) (string-after input_string char)))
	 (set! has_matched nil)))
    input_string))

(provide 'marathi_token)
