<?php
/**
 * @file
 * Views integration.
 */

/**
 * Implements hook_views_data().
 */
function twilio_views_data() {
  $data['twilio_user']['table']['group'] = t('Twilio');
  $data['twilio_user']['table']['join'] = array(
    'users' => array(
      'left_field' => 'uid',
      'field' => 'uid',
    ),
  );
  $data['twilio_user']['number'] = array(
    'title' => t('Phone number'),
    'help' => t('The phone number associated with the user account.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['twilio_user']['country'] = array(
    'title' => t('Country code'),
    'help' => t('The country code associated with the user account.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  return $data;
}
